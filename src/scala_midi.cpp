/****************************************************************
          libscala-file, (C) 2020 Mark Conway Wirt
         scala_midi.cpp, (C) 2025 Kenny Phelps-McKeown
            See LICENSE for licensing terms (MIT)
****************************************************************/

#include <cmath>
#include "scala_file.hpp"

namespace scala {

    degree convert_midi_inner(int note, scale scl, kbm map){
        if( !(map.first_note <= note <= map.last_note) )
            throw std::runtime_error("MIDI note out of kbm range");

        int relative = note - map.middle_note;
        int scale_degree = relative % map.map_size;
        // Modulus with negative numbers doesn't behave how we need it to
        if(scale_degree < 0) scale_degree += map.map_size;
        int num_octaves = std::floor(static_cast<double>(relative) /
				     static_cast<double>(map.map_size));
        degree octave_value = scl.degrees[map.octave_degree];

        if(map.mapping[scale_degree] == KBM_NON_ENTRY)
            throw std::runtime_error("MIDI note maps to non-entry");

        degree ret = scl.degrees[ map.mapping[scale_degree] ];

        // Adjust for middle_note != reference_note
        // so the ratio can just be multiplied by reference_frequency
        if(note != map.reference_note)
            ret /= convert_midi_inner(map.reference_note, scl, map);

        if(num_octaves > 0)
            for(int i=0; i<num_octaves; i++) ret *= octave_value;
        else
            for(int i=0; i>num_octaves; i--) ret /= octave_value;

        return ret;
    }

    degree convert_midi(int note, scale scl, kbm map){
        if(note == map.reference_note)
            return degree(1,1);
        else
            return convert_midi_inner(note, scl, map);
    }

}
