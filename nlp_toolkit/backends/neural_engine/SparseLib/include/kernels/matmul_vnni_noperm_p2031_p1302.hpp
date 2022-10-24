//  Copyright (c) 2022 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#ifndef ENGINE_SPARSELIB_INCLUDE_KERNELS_MATMUL_VNNI_NOPERM_P2031_P1302_HPP_
#define ENGINE_SPARSELIB_INCLUDE_KERNELS_MATMUL_VNNI_NOPERM_P2031_P1302_HPP_

#include <glog/logging.h>
#include <memory>
#include <vector>
#include <algorithm>

#include "cpu_isa.hpp"
#include "jit_domain/jit_matmul_vnni_noperm_p2031_p1302.hpp"
#include "kernel.hpp"
#include "kernel_desc.hpp"

namespace jd {
// By convention,
//   1. xxxx_kd_t is the descriptor of a specific derived primitive/kernel.
//   2. xxxx_k_t is a specific derived primitive/kernel.
//   3. jit_xxxx_t is JIT assembly implementation of a specific derived
//   primitive/kernel. where, "xxxx" represents an algorithm, such as brgemm,
//   GEMM and so on.
class matmul_vnni_noperm_p2031_p1302_k_t;

/**
 * @brief a derived kernel descriptor. matmul_param_t is its class member.
 */
class matmul_vnni_noperm_p2031_p1302_kd_t : public kernel_desc_t {
 public:
  explicit matmul_vnni_noperm_p2031_p1302_kd_t(const jd::operator_desc& op_desc)
      : kernel_desc_t(kernel_kind::sparse_matmul), op_desc_(op_desc) {}
  virtual ~matmul_vnni_noperm_p2031_p1302_kd_t() {}

  bool init() override;

  // kernel_desc_t::create_primitive() override.
  DECLARE_COMMON_PD_T(matmul_vnni_noperm_p2031_p1302_k_t, matmul_vnni_noperm_p2031_p1302_kd_t);

  const jd::operator_desc& operator_desc() const override { return op_desc_; }
  const ssd::matmul_param_t& jit_param() const { return jit_param_; }

  inline std::vector<dim_t> shape() const {
    std::vector<dim_t> result(op_desc_.tensor_descs()[ssd::SRC0].shape());  // bs0 bs1 M K
    result.push_back(op_desc_.tensor_descs()[ssd::SRC1].shape()[1]);        // N
    return result;
  }

 private:
  bool matmul_params_init(const jd::operator_desc& op_desc);

  jd::operator_desc op_desc_;
  ssd::matmul_param_t jit_param_;
};

/**
 * @brief a derived kernel. kd_t and jit_domain are its class members.
 */
class matmul_vnni_noperm_p2031_p1302_k_t : public kernel_t {
 public:
  using kd_t = matmul_vnni_noperm_p2031_p1302_kd_t;
  explicit matmul_vnni_noperm_p2031_p1302_k_t(const std::shared_ptr<const kd_t>& kd);
  virtual ~matmul_vnni_noperm_p2031_p1302_k_t() {
    if (jit_ker_ != nullptr) {
      delete jit_ker_;
      jit_ker_ = nullptr;
    }
  }

  // Delete move constructor and move operator
  matmul_vnni_noperm_p2031_p1302_k_t(matmul_vnni_noperm_p2031_p1302_k_t&& other) = delete;
  matmul_vnni_noperm_p2031_p1302_k_t& operator=(matmul_vnni_noperm_p2031_p1302_k_t&& other) = delete;
  // Delete copy constructor and copy operator
  matmul_vnni_noperm_p2031_p1302_k_t(const matmul_vnni_noperm_p2031_p1302_k_t& other) = delete;
  matmul_vnni_noperm_p2031_p1302_k_t& operator=(const matmul_vnni_noperm_p2031_p1302_k_t& other) = delete;

  bool init() override;
  bool execute(const std::vector<const void*>& rt_data) const override;
  const std::shared_ptr<const kd_t> derived_kd() const { return std::static_pointer_cast<const kd_t>(kd_); }

 private:
  bool matmul_kernel_create(jit_matmul_vnni_noperm_p2031_p1302_t** ker_pp, const ssd::matmul_param_t& param);

 private:
  jit_matmul_vnni_noperm_p2031_p1302_t* jit_ker_ = nullptr;
  const std::vector<std::vector<dim_t>> t_shapes_;
  const std::vector<dim_t>& src0_perm_shape_;  // src0 perm none
  const std::vector<dim_t> src1_perm_shape_;   // src1 shape after perm2031
  const std::vector<dim_t> dst1_perm_shape_;   // dst1 shape before perm1302
  const dim_t M_, K_, N_;                      // dim of matrix multiplication
  const dim_t bs0_;                            // outer batch size dim
  const dim_t bs1_;                            // innter batch size dim
};

}  // namespace jd

#endif  // ENGINE_SPARSELIB_INCLUDE_KERNELS_MATMUL_VNNI_NOPERM_P2031_P1302_HPP_
