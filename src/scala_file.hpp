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

    struct degree {

        template<class... Ts>
        struct overload : Ts... { using Ts::operator()...; };

        enum class init_type {
            cents,
            ratio
        };

        std::variant<double, boost::rational<int>> ratio;

        degree (int n, int d){
            // Two inputs: a ratio
            ratio = boost::rational<int> (n, d);
        }

        degree (double value, init_type type = init_type::cents){
            // One input: cents (default)
            if(type == init_type::cents)
                ratio = std::pow(std::pow(2, 1.0 / 12.0), value/100.0);
            // Otherwise take directly as ratio
            else ratio = value;
        }

        explicit degree (boost::rational<int> q){
            ratio = q;
        }

        double get_ratio() {
            // Use to get the value
            return get_double();
        }

        double get_double() {
            using boost::rational, boost::rational_cast;
            return std::visit(overload{
                [](double& p)        { return p; },
                [](rational<int>& q) { return rational_cast<double>(q); }
            }, ratio);
        }

        degree pow (int exp){
            degree ret = *this;
            if(exp > 0) for(int i=0; i<exp; i++) ret *= *this;
            if(exp < 0) {
                degree recip = degree(boost::rational<int>(1,1)) / *this;
                for(int i=0; i>exp; i--) ret *= recip;
            }
            return ret;
        }

        /* Operator overloads take care of type conversions automatically
           so we can * and / scala::degree's as if they're simply ratios
           and count on the precision of the rational type to be preserved
           until an irrational (cents-based) value is introduced */

        degree operator* (const degree& d) const {
            using boost::rational, boost::rational_cast;
            return std::visit(overload{
                [](const rational<int>& q1, const rational<int>& q2) {
                    return degree(q1 * q2); },
                [](const rational<int>& q, const double& p) {
                    return degree(rational_cast<double>(q) * p,
                                  init_type::ratio); },
                [](const double& p, const rational<int>& q) {
                    return degree(p * rational_cast<double>(q),
                                  init_type::ratio); },
                [](const double& p1, const double& p2) {
                    return degree(p1 * p2, init_type::ratio); }
            }, this->ratio, d.ratio);
        }

        degree operator/ (const degree& d) const {
            using boost::rational, boost::rational_cast, std::log2;
            return std::visit(overload{
                [](const rational<int>& q1, const rational<int>& q2) {
                    return degree(q1 / q2); },
                [](const rational<int>& q, const double& p) {
                    return degree(rational_cast<double>(q) / p,
                                  init_type::ratio); },
                [](const double& p, const rational<int>& q) {
                    return degree(p / rational_cast<double>(q),
                                  init_type::ratio); },
                [](const double& p1, const double& p2) {
                    return degree(p1 / p2, init_type::ratio); }
            }, this->ratio, d.ratio);
        }

        degree& operator*= (const degree& d) {
            using boost::rational, boost::rational_cast;
            std::visit(overload{
                [](rational<int>& q1, const rational<int>& q2) {
                    q1 *= q2; },
                [&](rational<int>& q, const double& p) {
                    ratio.emplace<double>(rational_cast<double>(q) * p); },
                [](double& p, const rational<int>& q) {
                    p *= rational_cast<double>(q); },
                [](double& p1, const double& p2) {
                    p1 *= p2; }
            }, ratio, d.ratio);
            return *this;
        }

        degree& operator/= (const degree& d) {
            using boost::rational, boost::rational_cast;
            std::visit(overload{
                [](rational<int>& q1, const rational<int>& q2) {
                    q1 /= q2; },
                [&](rational<int>& q, const double& p) {
                    ratio.emplace<double>(rational_cast<double>(q) / p); },
                [](double& p, const rational<int>& q) {
                    p /= rational_cast<double>(q); },
                [](double& p1, const double& p2) {
                    p1 /= p2; }
            }, ratio, d.ratio);
            return *this;
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
    degree convert_midi(int note, scale scl, kbm map);
}
