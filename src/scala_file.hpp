/****************************************************************
          libscala-file, (C) 2020 Mark Conway Wirt
            See LICENSE for licensing terms (MIT)
 ****************************************************************/
#pragma once

#include <fstream>
#include <math.h>
#include <vector>
#include <variant>
#include <boost/rational.hpp>


// Define this before compilation if you want a stricter adherence 
// to the specification.

// #define SCALA_STRICT

#define KBM_NON_ENTRY -1

namespace scala {

    struct real : public std::variant<std::monostate,
                                      boost::rational<int>,
                                      double> {

        using std::variant<std::monostate,
                           boost::rational<int>,
                           double>::variant;
        using std::variant<std::monostate,
                           boost::rational<int>,
                           double>::operator=;

        template<class... Ts>
        struct overload : Ts... { using Ts::operator()...; };

        double get_double() {
            // Converts boost::rational to double automatically
            using boost::rational, boost::rational_cast;
            return std::visit(overload{
                [](double& p) {
                    return p; },
                [](rational<int>& q) {
                    return rational_cast<double>(q); },
                [](std::monostate& m) {
                    throw std::bad_variant_access(); return 0.; }
            }, *this);
        }

        boost::rational<int>& get_rational() {
            // Throws std::bad_variant_access if not rational
            return std::get<boost::rational<int>>(*this);
        }

        bool is_rational() {
            return std::holds_alternative<boost::rational<int>>(*this);
        }

        /* Operator overloads take care of type conversions automatically
           so we can * and / reals as if they're simply ratios and count
           on the precision of the rational type to be preserved until an
           irrational (cents-based) value is introduced */

        real& operator*= (const real& rhs) {
            using boost::rational, boost::rational_cast;
            std::visit(overload{
                [](rational<int>& q1, const rational<int>& q2) {
                    q1 *= q2; },
                [this](rational<int>& q, const double& p) {
                    this->emplace<double>(rational_cast<double>(q) * p); },
                [](double& p, const rational<int>& q) {
                    p *= rational_cast<double>(q); },
                [](double& p1, const double& p2) {
                    p1 *= p2; },
                [](auto& a1, const auto& a2) {
                    throw std::bad_variant_access(); }
            }, *this, rhs);
            return *this;
        }

        real& operator/= (const real& rhs) {
            using boost::rational, boost::rational_cast;
            std::visit(overload{
                [](rational<int>& q1, const rational<int>& q2) {
                    q1 /= q2; },
                [this](rational<int>& q, const double& p) {
                    this->emplace<double>(rational_cast<double>(q) / p); },
                [](double& p, const rational<int>& q) {
                    p /= rational_cast<double>(q); },
                [](double& p1, const double& p2) {
                    p1 /= p2; },
                [](auto& a1, const auto& a2) {
                    throw std::bad_variant_access(); }
            }, *this, rhs);
            return *this;
        }

    };

    inline real operator* (const real& lhs, const real& rhs) {
        real ret = lhs;
        ret *= rhs;
        return ret;
    }

    inline real operator/ (const real& lhs, const real& rhs) {
        real ret = lhs;
        ret /= rhs;
        return ret;
    }

    struct degree {

        real ratio;

        degree (int n, int d)
            // Two inputs: a ratio
            : ratio( boost::rational<int>(n,d) ) {}

        explicit degree (double cents)
            // One input: cents
            : ratio( pow(pow(2, 1.0 / 12.0), cents/100.0) ) {}

        double get_ratio() {
            // Use to get the value
            return ratio.get_double();
        }

    };

    struct scale {
        
        std::vector <degree> degrees;

        scale () {
            // The first degree is a scala file is always implicit. Make it explicit.
            degrees.push_back( *(new degree(1,1)));
        }

        ~scale(){
            degrees.clear();
            std::vector<degree>().swap(degrees);
        }

        scale(const scale &s2) {
            for (size_t i = 0; i < s2.degrees.size(); i++) {
                degrees.push_back(s2.degrees[i]); 
            }
        }

        scale& operator=(const scale &s2){
            std::vector <degree> new_degrees;
            for (size_t i = 0; i < s2.degrees.size(); i++) {
                new_degrees.push_back(s2.degrees[i]);
            }
            degrees.swap(new_degrees);
            new_degrees.clear();
            return *this;
        }

        void add_degree(degree d) {
            degrees.push_back(d);
        }

        double get_ratio(size_t i){
            return degrees[i].get_ratio();
        }

        size_t get_scale_length() {
            return degrees.size();
        }

    };

    struct kbm {
        double reference_frequency;
        int map_size;
        int first_note;
        int last_note;
        int middle_note;
        int reference_note;
        int octave_degree;
        std::vector <int> mapping;

        kbm(){
            map_size = 0;
            first_note = 0;
            last_note = 0;
            middle_note = 0;
            reference_note = 0;
            reference_frequency = 0.0;
            octave_degree = 0;
        }

        ~kbm(){
            mapping.clear();
            std::vector<int>().swap(mapping);
        }

        kbm( const kbm &k2){
            for (size_t i = 0; i < k2.mapping.size(); i++) {
                mapping.push_back(k2.mapping[i]); 
            }
            map_size = k2.map_size;
            first_note = k2.first_note;
            last_note = k2.last_note;
            middle_note = k2.middle_note;
            reference_note = k2.reference_note;
            reference_frequency = k2.reference_frequency;
            octave_degree = k2.octave_degree;
        }

        kbm& operator=(const kbm &k2){
            std::vector <int> new_mapping;
            for (size_t i = 0; i < k2.mapping.size(); i++) {
                new_mapping.push_back(k2.mapping[i]);
            }
            mapping.swap(new_mapping);
            new_mapping.clear();

            map_size = k2.map_size;
            first_note = k2.first_note;
            last_note = k2.last_note;
            middle_note = k2.middle_note;
            reference_note = k2.reference_note;
            reference_frequency = k2.reference_frequency;
            octave_degree = k2.octave_degree;

            return *this;
        }

        void add_mapping(int n) {
            mapping.push_back(n);
        }

    };

    scale read_scl(std::ifstream& input_file);
    kbm read_kbm(std::ifstream& input_file);
    real convert_midi(int note, scale scl, kbm map);
}
