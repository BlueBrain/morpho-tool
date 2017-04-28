/*
 * Copyright (C) 2017 Adrien Devresse <adrien.devresse@epfl.ch>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
*/


#include <morpho/morpho_circuit.hpp>

#include <morpho/morpho_h5_v1.hpp>
#include <morpho/morpho_transform_filters.hpp>


namespace morpho{

namespace circuit{


circuit_reader::circuit_reader(const std::string & filename_mvd3, const std::string & morpho_directory) :
    _filename(filename_mvd3),
    _morpho_directory(morpho_directory)
{

}




std::vector<morpho_tree> circuit_reader::create_all_morpho_tree() const{
    std::vector<morpho_tree> circuit_morpho_tree;

    std::unique_ptr<MVD3::MVD3File> file;

    try{
        file.reset(new MVD3::MVD3File(_filename));

    } catch(std::exception & e){
        throw std::runtime_error(hadoken::format::scat("Impossible to open circuit file ", _filename, " \n", e.what()));
    }

    auto all_morphologies_name = file->getMorphologies();

    auto all_positions = file->getPositions();
    auto all_rotations = file->getRotations();

    assert(all_positions.shape()[0] == all_rotations.shape()[0]);
    assert(all_morphologies.size() == all_rotations.shape()[0]);

    circuit_morpho_tree.reserve(all_morphologies_name.size());


    for(std::size_t i =0; i < all_morphologies_name.size(); ++i){
        const std::string morphology_name = hadoken::format::scat(_morpho_directory,"/",all_morphologies_name[i]);
        try{
            h5_v1::morpho_reader reader(morphology_name);

            morpho_tree raw_morpho = reader.create_morpho_tree();

            morpho_tree morpho_transposed =
                morpho_transform(
                    raw_morpho,
                    {
                        std::make_shared<transpose_operation>(
                            transpose_operation::vector3d({ all_positions[i][0], all_positions[i][1], all_positions[i][2] }),
                            transpose_operation::quaternion3d({ all_rotations[i][0], all_rotations[i][1], all_rotations[i][2],  all_rotations[i][3] })
                        )
                    }
           );

            circuit_morpho_tree.emplace_back(std::move(morpho_transposed));

        } catch(std::exception & e){
            throw std::runtime_error(hadoken::format::scat("Impossible to open morphology ", morphology_name,
                                                           " in circuit ",_filename,
                                                            "\n", e.what()));
        }

    }

    return circuit_morpho_tree;
}


} // circuit

} //morpho

