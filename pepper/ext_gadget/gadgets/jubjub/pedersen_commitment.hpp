/*    
    copyright 2018 to the baby_jubjub_ecc Authors

    This file is part of baby_jubjub_ecc.

    baby_jubjub_ecc is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    baby_jubjub_ecc is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with baby_jubjub_ecc.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef JUBJUB_PEDERSEN_HPP_
#define JUBJUB_PEDERSEN_HPP_

#include <cassert>
#include <memory>

#include <gadgets/jubjub/curve.hpp>
#include <gadgets/jubjub/fixed_base_mul.hpp>

namespace ethsnarks {


class  pedersen_commitment : public GadgetT {

private:
    /* no internal variables */
public:

    VariableT a;
    VariableT d;
    jubjub::Params params;
    FieldT base_x;
    FieldT base_y;
    FieldT H_x;
    FieldT H_y;

    //input variables 
    VariableT commitment_x;
    VariableT commitment_y;
    VariableArrayT m;
    VariableArrayT r;

    std::shared_ptr<pointAddition> jubjub_pointAddition;
    std::shared_ptr<jubjub::fixed_base_mul> jubjub_pointMultiplication_lhs;
    std::shared_ptr<jubjub::fixed_base_mul> jubjub_pointMultiplication_rhs;


     pedersen_commitment(ProtoboardT &pb,
                   const VariableArrayT &left, const VariableArrayT &right,
                   const VariableT &commitment_x, const VariableT &commitment_y
                   );

    void generate_r1cs_constraints();
    void generate_r1cs_witness();
};

// namespace ethsnarks
}

// JUBJUB_PEDERSEN_HPP_
#endif
