#ifndef IDEEP_OPERATORS_INNER_PRODUCT_MATMUL_HPP
#define IDEEP_OPERATORS_INNER_PRODUCT_MATMUL_HPP

namespace ideep {

struct matmul_forward_params {
  dnnl::matmul::primitive_desc pd;
  dnnl::matmul primitive;
  attr_t op_attr;
  // bias_attr contains requantization scales for bias
  attr_t bias_attr;
  scale_t dst_scales;
  attr_t src_attr;
  attr_t weights_attr;
  tensor::desc src_desc;
  tensor::desc weights_desc;
  tensor scales_m;
  tensor src_zero_point_m;
  tensor wei_zero_point_m;
  tensor dst_zero_point_m;
};

struct matmul_forward : public dnnl::matmul,
                        utils::computation_cache<dnnl::matmul::primitive_desc> {
  using super = dnnl::matmul;

  // With or without bias
  // Zero points are passed explicitly as arguments for quantization
  static void compute_v2(
      const tensor& src,
      const tensor& weights,
      const tensor& bias,
      bool with_bias,
      tensor& dst,
      const float dst_coeff = 1.0f,
      const float sum_coeff = 1.0f,
      const scale_t& src_scales = scale_t(),
      const scale_t& weights_scales = scale_t(),
      const scale_t& dst_scales = scale_t(),
      const zero_point_t& src_zero_points = zero_point_t(),
      const zero_point_t& dst_zero_points = zero_point_t(),
      const attr_t& attr = attr_t(),
      const data_type dst_type = data_type::undef,
      const lowp_kind alowp_kind = u8s8,
      const engine& aengine = engine::cpu_engine()) {
    if (with_bias) {
      compute_impl</*with_bias=*/true>(src, weights, bias, dst, dst_coeff, sum_coeff,
                                       src_scales, weights_scales, dst_scales,
                                       src_zero_points, dst_zero_points,
                                       attr, dst_type, alowp_kind, aengine);
    } else {
      compute_impl</*with_bias=*/false>(src, weights, bias, dst, dst_coeff,
                                        sum_coeff, src_scales, weights_scales, dst_scales,
                                        src_zero_points, dst_zero_points,
                                        attr, dst_type, alowp_kind, aengine);
    }
  }

  // Deprecated
  // With bias. Zero points are passed explicitly as arguments for quantization
  static void compute_v2(
      const tensor& src,
      const tensor& weights,
      const tensor& bias,
      tensor& dst,
      const float dst_coeff = 1.0f,
      const float sum_coeff = 1.0f,
      const scale_t& src_scales = scale_t(),
      const scale_t& weights_scales = scale_t(),
      const scale_t& dst_scales = scale_t(),
      const zero_point_t& src_zero_points = zero_point_t(),
      const zero_point_t& dst_zero_points = zero_point_t(),
      const attr_t& attr = attr_t(),
      const data_type dst_type = data_type::undef,
      const lowp_kind alowp_kind = u8s8,
      const engine& aengine = engine::cpu_engine()) {
    compute_impl</*with_bias=*/true>(src, weights, bias, dst, dst_coeff, sum_coeff,
                                     src_scales, weights_scales, dst_scales,
                                     src_zero_points, dst_zero_points,
                                     attr, dst_type, alowp_kind, aengine);
  }

  // Deprecated
  // Without bias. Zero points are passed explicitly as arguments for quantization
  static void compute_v2(
      const tensor& src,
      const tensor& weights,
      tensor& dst,
      const float dst_coeff = 1.0f,
      const float sum_coeff = 1.0f,
      const scale_t& src_scales = scale_t(),
      const scale_t& weights_scales = scale_t(),
      const scale_t& dst_scales = scale_t(),
      const zero_point_t& src_zero_points = zero_point_t(),
      const zero_point_t& dst_zero_points = zero_point_t(),
      const attr_t& attr = attr_t(),
      const data_type dst_type = data_type::undef,
      const lowp_kind alowp_kind = u8s8,
      const engine& aengine = engine::cpu_engine()) {
    static tensor dummy_bias;
    compute_impl</*with_bias=*/false>(src, weights, dummy_bias, dst, dst_coeff,
                                      sum_coeff, src_scales, weights_scales, dst_scales,
                                      src_zero_points, dst_zero_points,
                                      attr, dst_type, alowp_kind, aengine);
  }

  // Deprecated. With bias. Set zero points to tensors for quantization.
  static void compute(
      const tensor& src,
      const tensor& weights,
      const tensor& bias,
      tensor& dst,
      const float dst_coeff = 1.0f,
      const float sum_coeff = 1.0f,
      const scale_t& src_scales = scale_t(),
      const scale_t& weights_scales = scale_t(),
      const scale_t& dst_scales = scale_t(),
      const attr_t& attr = attr_t(),
      const data_type dst_type = data_type::undef,
      const lowp_kind alowp_kind = u8s8,
      const engine& aengine = engine::cpu_engine()) {
    compute_impl</*with_bias=*/true>(src, weights, bias, dst, dst_coeff, sum_coeff,
                                     src_scales, weights_scales, dst_scales,
                                     zero_point_t(), zero_point_t(),
                                     attr, dst_type, alowp_kind, aengine);
  }

  // Deprecated. Without bias. Set zero points to tensors for quantization.
  static void compute(
      const tensor& src,
      const tensor& weights,
      tensor& dst,
      const float dst_coeff = 1.0f,
      const float sum_coeff = 1.0f,
      const scale_t& src_scales = scale_t(),
      const scale_t& weights_scales = scale_t(),
      const scale_t& dst_scales = scale_t(),
      const attr_t& attr = attr_t(),
      const data_type dst_type = data_type::undef,
      const lowp_kind alowp_kind = u8s8,
      const engine& aengine = engine::cpu_engine()) {
    static tensor dummy_bias;
    compute_impl</*with_bias=*/false>(src, weights, dummy_bias, dst, dst_coeff,
                                      sum_coeff, src_scales, weights_scales, dst_scales,
                                      zero_point_t(), zero_point_t(),
                                      attr, dst_type, alowp_kind, aengine);
  }

  static void prepare(matmul_forward_params& param,
                      const tensor& src,
                      const tensor& weights,
                      const tensor& bias,
                      bool with_bias,
                      tensor& dst,
                      const float dst_coeff = 1.0f,
                      const float sum_coeff = 1.0f,
                      const scale_t& src_scales = scale_t(),
                      const scale_t& weights_scales = scale_t(),
                      const scale_t& dst_scales = scale_t(),
                      const zero_point_t& src_zero_points = zero_point_t(),
                      const zero_point_t& dst_zero_points = zero_point_t(),
                      const attr_t& attr = attr_t(),
                      const data_type dst_type = data_type::undef,
                      const lowp_kind alowp_kind = u8s8,
                      const engine& aengine = engine::cpu_engine()) {
    do_prepare(param, PrepareStatic, src, weights, bias, with_bias, dst, dst_coeff, sum_coeff,
               src_scales, weights_scales, dst_scales, src_zero_points, dst_zero_points,
               attr, dst_type, alowp_kind, aengine);
  }

  static void compute(const matmul_forward_params& param,
                      const tensor& src,
                      const tensor& weights,
                      const tensor& bias,
                      bool with_bias,
                      tensor& dst) {
    do_compute(param, ComputeStatic, src, weights, bias, with_bias, dst);
  }

  static void prepare_dynamic(matmul_forward_params& param,
                      const tensor& src,
                      const tensor& weights,
                      const tensor& bias,
                      bool with_bias,
                      tensor& dst,
                      const float dst_coeff = 1.0f,
                      const float sum_coeff = 1.0f,
                      const scale_t& src_scales = scale_t(),
                      const scale_t& weights_scales = scale_t(),
                      const scale_t& dst_scales = scale_t(),
                      const zero_point_t& src_zero_points = zero_point_t(),
                      const zero_point_t& dst_zero_points = zero_point_t(),
                      const attr_t& attr = attr_t(),
                      const data_type dst_type = data_type::undef,
                      const lowp_kind alowp_kind = u8s8,
                      const engine& aengine = engine::cpu_engine()) {
    do_prepare(param, PrepareDynamic, src, weights, bias, with_bias, dst, dst_coeff, sum_coeff,
        src_scales, weights_scales, dst_scales, src_zero_points, dst_zero_points,
        attr, dst_type, alowp_kind, aengine);
  }

 static void compute_dynamic(
     const matmul_forward_params& param,
     const tensor& src,
     const tensor& weights,
     const tensor& bias,
     bool with_bias,
     tensor& dst,
     const float dst_coeff = 1.0f,
     const float sum_coeff = 1.0f,
     const scale_t& src_scales = scale_t(),
     const scale_t& weights_scales = scale_t(),
     const scale_t& dst_scales = scale_t(),
     const zero_point_t& src_zero_points = zero_point_t(),
     const zero_point_t& dst_zero_points = zero_point_t(),
     const attr_t& attr = attr_t(),
     const data_type dst_type = data_type::undef,
     const lowp_kind alowp_kind = u8s8,
     const engine& aengine = engine::cpu_engine()) {
    // Call do_prepare here to calculate scales and zero points.
    matmul_forward_params param_for_compute;
    do_prepare(param_for_compute, ComputeDynamic, src, weights, bias, with_bias, dst, dst_coeff, sum_coeff,
        src_scales, weights_scales, dst_scales, src_zero_points, dst_zero_points,
        attr, dst_type, alowp_kind, aengine);
    param_for_compute.pd = param.pd;
    param_for_compute.primitive = param.primitive;
    param_for_compute.op_attr = param.op_attr;
    do_compute(param_for_compute, ComputeDynamic, src, weights, bias, with_bias, dst);
  }

  static tensor::desc expected_weights_desc(
      const dims& weights_dims,
      data_type dtype = data_type::f32,
      data_type x_dtype = data_type::f32,
      const engine& aengine = engine::cpu_engine()) {
    auto ndims = weights_dims.size();
    auto x_dims = weights_dims;
    x_dims[ndims-2] = DNNL_RUNTIME_DIM_VAL;
    x_dims[ndims-1] = weights_dims[ndims-2];
    auto y_dims = {x_dims[0], weights_dims[1]};
    if (ndims == 3) 
        y_dims = {x_dims[0], x_dims[1], weights_dims[2]};
    auto y_dtype = (dtype != data_type::s8) ? dtype : data_type::s32;

    IDEEP_ENFORCE(x_dims.size() == weights_dims.size(),
                  "Invalid dims for data and weights");
    tensor::desc x_desc(x_dims, x_dtype, ndims == 2 ? tag::ab : tag::abc);
    tensor::desc y_desc(y_dims, y_dtype, ndims == 2 ? tag::ab : tag::abc);
    tensor::desc weights_desc(weights_dims , dtype, tag::any);
    attr_t attr;
    attr.set_output_scales(/* mask */ (1 << 1), {DNNL_RUNTIME_F32_VAL});
    attr.set_zero_points(DNNL_ARG_SRC, /* mask */ 0, {DNNL_RUNTIME_S32_VAL});
    attr.set_zero_points(DNNL_ARG_DST, /* mask */ 0, {DNNL_RUNTIME_S32_VAL});
    auto pd = primitive_desc({x_desc, weights_desc, y_desc}, attr, aengine);
    return pd.weights_desc();
  }

private:
enum TaskType {
  PrepareStatic,
  PrepareDynamic,
  ComputeStatic,
  ComputeDynamic,
};

  template <bool with_bias>
 static void compute_impl(const tensor& src,
                          const tensor& weights,
                          const tensor& bias,
                          tensor& dst,
                          const float dst_coeff = 1.0f,
                          const float sum_coeff = 1.0f,
                          const scale_t& src_scales = scale_t(),
                          const scale_t& weights_scales = scale_t(),
                          const scale_t& dst_scales = scale_t(),
                          const zero_point_t& src_zero_points = zero_point_t(),
                          const zero_point_t& dst_zero_points = zero_point_t(),
                          const attr_t& attr = attr_t(),
                          const data_type dst_type = data_type::undef,
                          const lowp_kind alowp_kind = u8s8,
                          const engine& aengine = engine::cpu_engine()) {
    matmul_forward_params param;
    do_prepare(param, PrepareStatic, src, weights, bias, with_bias, dst, dst_coeff, sum_coeff,
               src_scales, weights_scales, dst_scales, src_zero_points, dst_zero_points,
               attr, dst_type, alowp_kind, aengine);
    do_compute(param, ComputeStatic, src, weights, bias, with_bias, dst);
  }

 static void do_prepare(matmul_forward_params& param,
                        TaskType task,
                        const tensor& src,
                        const tensor& weights,
                        const tensor& bias,
                        bool with_bias,
                        tensor& dst,
                        const float dst_coeff = 1.0f,
                        const float sum_coeff = 1.0f,
                        const scale_t& src_scales = scale_t(),
                        const scale_t& weights_scales = scale_t(),
                        const scale_t& dst_scales = scale_t(),
                        const zero_point_t& src_zero_points = zero_point_t(),
                        const zero_point_t& dst_zero_points = zero_point_t(),
                        const attr_t& attr = attr_t(),
                        const data_type dst_type = data_type::undef,
                        const lowp_kind alowp_kind = u8s8,
                        const engine& aengine = engine::cpu_engine()) {
   IDEEP_ENFORCE(src.ndims() == weights.ndims(), "Invalid dims in src or weights");

   tensor::desc src_desc, weights_desc, bias_desc;
   attr_t op_attr = attr, src_attr, weights_attr, bias_attr;
   scale_t dst_scales_in;
   auto dst_data_type = data_type::f32;

   bool is_dynamic = (task == PrepareDynamic || task == ComputeDynamic);
   const int64_t runtime_bs = DNNL_RUNTIME_DIM_VAL;
   tensor::dims src_dims = src.get_dims();
   if (task == PrepareDynamic) {
     src_dims[0] = runtime_bs;
   }
   tensor::dims dst_dims = {src_dims[0], weights.get_dim(1)};
   auto ndims = weights.ndims();
   if (ndims == 3)
       dst_dims = {src_dims[0], src.get_dim(1), weights.get_dim(2)};

   auto& weights_scales_in =
       weights.has_scale() ? weights.get_scale() : weights_scales;
   tensor scales_m, src_zero_point_m, wei_zero_point_m, dst_zero_point_m;
   if (!weights_scales_in.empty()) {
     IDEEP_ENFORCE(alowp_kind == u8s8 || alowp_kind == s8s8,
                   "Unsupported lowp kind");

     auto src_scales_in =
         src.has_scale() ? src.get_scale()
                         : (src_scales.empty() ? IDEEP_DEF_SCALE : src_scales);
     auto src_data_type = (alowp_kind == u8s8) ? data_type::u8 : data_type::s8;
     std::vector<int64_t> src_strides = (ndims == 3) ?
         std::vector<int64_t>({src_dims[1] * src_dims[2], src_dims[1], 1}) :
         std::vector<int64_t>({src_dims[1], 1});
     src_desc = is_dynamic ?
                tensor::desc(src_dims, src_data_type, src_strides) :
                tensor::desc(src_dims, src_data_type, tag::any);
     if (src.get_data_type() == data_type::f32) {
       src_attr = {0, src_scales_in};
     }

     int scale_size = (weights_scales_in.size() > 1) ? weights.get_dim(1) : 1;
     weights_desc = weights.get_desc();
     if (weights.get_data_type() == data_type::f32) {
       weights_attr = {utils::tensor_scale_mask(scale_size, false),
                       weights_scales_in};
     }

     // determine dst data type
     if (dst.get_data_type() != data_type::undef) {
       dst_data_type = dst.get_data_type();
     } else if (dst_scales.empty() || dst_scales == IDEEP_DEF_SCALE) {
       dst_data_type = data_type::f32;
     } else {
       dst_data_type = data_type::u8;
     }

     // fill primitive attr
     scale_t op_scales(scale_size), bias_scales(scale_size);
     dst_scales_in = (dst_scales.empty() || dst_data_type == data_type::f32)
                          ? IDEEP_DEF_SCALE
                          : dst_scales;
     const zero_point_t default_zero_points = zero_point_t(1);
     const auto& src_zero_point = src.has_zero_point() ? src.get_zero_point() :
                                  src_zero_points.empty() ? default_zero_points : src_zero_points;
     const auto src_zero_point_size = static_cast<dim>(src_zero_point.size());
     const auto& dst_zero_point = dst.has_zero_point() ? dst.get_zero_point() :
         dst_zero_points.empty() ? default_zero_points : dst_zero_points;
     const auto dst_zero_point_size = static_cast<dim>(dst_zero_point.size());
     IDEEP_ENFORCE(src_zero_point_size == 1 && dst_zero_point_size == 1,
                   "DNNL only support 1-dim zero_point");
     const auto& wei_zero_point = weights.has_zero_point() ?
                                  weights.get_zero_point() : default_zero_points;
     const dim wei_zero_point_size = 1;

     if (attr.has_op_kind(kind::sum)) {
       float sum_scale =
           sum_coeff * dst_scales_in[0] / (dst.has_scale() ? dst.get_scale()[0] : 1.0f);
       op_attr = attr_t::fuse_sum(sum_scale);
     }

     auto bias_scales_in =
         bias.has_scale() ? bias.get_scale() : IDEEP_DEF_SCALE;
     bias_scales_in = bias_scales_in.size() == 1 ?
	     std::vector<float>(scale_size, bias_scales_in[0]) : bias_scales_in;
     bool flag_runtime = is_dynamic;
     if (flag_runtime) {
       if (task == PrepareDynamic) {
         op_attr.set_output_scales(utils::op_scale_mask(scale_size), {DNNL_RUNTIME_F32_VAL});
         op_attr.set_zero_points(DNNL_ARG_SRC, utils::tensor_zp_mask(1), {DNNL_RUNTIME_S32_VAL});
         op_attr.set_zero_points(DNNL_ARG_WEIGHTS, utils::tensor_zp_mask(1), {DNNL_RUNTIME_S32_VAL});
         if (dst_data_type != data_type::f32) {
           op_attr.set_zero_points(DNNL_ARG_DST, utils::tensor_zp_mask(1), {DNNL_RUNTIME_S32_VAL});
         }
       } else { // task == ComputeDynamic
         tensor::desc scales_desc = {{scale_size}, data_type::f32, {1}};
         scales_m.init(scales_desc, aengine);
         auto s = reinterpret_cast<float *>(scales_m.get_data_handle());
         for (memory::dim i = 0; i < scale_size; ++i) {
           bias_scales[i] = src_scales_in[0] * weights_scales_in[i] / (dst_coeff * bias_scales_in[i]);
           s[i] = dst_coeff * dst_scales_in[0] / (src_scales_in[0] * weights_scales_in[i]);
         }
         if (src.get_data_type() == data_type::f32) {
           // Set zero point for reorder (quantization). 1st arg should be DNNL_ARG_DST rather than DNNL_ARG_SRC
           src_attr.set_zero_points(DNNL_ARG_DST,
                                    utils::tensor_zp_mask(src_zero_point.size()), src_zero_point);
         }

         tensor::desc src_zero_point_desc = {{src_zero_point_size}, data_type::s32, {1}};
         src_zero_point_m.init(src_zero_point_desc, aengine);
         auto src_z = reinterpret_cast<int32_t *>(src_zero_point_m.get_data_handle());
         for (memory::dim i = 0; i < src_zero_point_size; ++i)
           src_z[i] = src_zero_point[i];

         tensor::desc wei_zero_point_desc = {{wei_zero_point_size}, data_type::s32, {1}};
         wei_zero_point_m.init(wei_zero_point_desc, aengine);
         auto wei_z = reinterpret_cast<int32_t *>(wei_zero_point_m.get_data_handle());
         for (memory::dim i = 0; i < wei_zero_point_size; ++i)
           wei_z[i] = wei_zero_point[i];

         if (dst_data_type != data_type::f32) {
           tensor::desc dst_zero_point_desc = {{dst_zero_point_size}, data_type::s32, {1}};
           dst_zero_point_m.init(dst_zero_point_desc, aengine);
           auto dst_z = reinterpret_cast<int32_t *>(dst_zero_point_m.get_data_handle());
           for (memory::dim i = 0; i < dst_zero_point_size; ++i)
             dst_z[i] = dst_zero_point[i];
         }
       }
     } else {
       for (int i = 0; i < scale_size; i++) {
         bias_scales[i] = src_scales_in[0] * weights_scales_in[i] / (dst_coeff * bias_scales_in[i]);
         op_scales[i] = dst_coeff * dst_scales_in[0] / (src_scales_in[0] * weights_scales_in[i]);
       }
       op_attr.set_output_scales(utils::op_scale_mask(scale_size), op_scales);
       op_attr.set_zero_points(DNNL_ARG_SRC,
                               utils::tensor_zp_mask(src_zero_point.size()), src_zero_point);
       if (src.get_data_type() == data_type::f32) {
         // Set zero point for reorder (quantization). 1st arg should be DNNL_ARG_DST rather than DNNL_ARG_SRC
         src_attr.set_zero_points(DNNL_ARG_DST,
                                  utils::tensor_zp_mask(src_zero_point.size()), src_zero_point);
       }
       op_attr.set_zero_points(DNNL_ARG_WEIGHTS,
                               utils::tensor_zp_mask(1), zero_point_t(1,wei_zero_point[0]));
       if (dst_data_type != data_type::f32) {
         op_attr.set_zero_points(DNNL_ARG_DST,
                                 utils::tensor_zp_mask(dst_zero_point.size()), dst_zero_point);
       }
     }

     if (with_bias) {
       tag bia_tag = bias.get_dims().size() == 2 ? tag::ab : tag::abc;
       bias_desc = {bias.get_dims(), data_type::f32, bia_tag}; // Use f32 instead of s32 to improve accuracy
       if (bias.get_data_type() != data_type::s32) {
         auto ndims = bias.get_dims().size();
         int mask = scale_size > 1 ? 1 << (ndims - 1) : 0;
         bias_attr = {mask, bias_scales};
       }
     }
   } else {
     if (src.has_scale()) {
       auto src_scale = src.get_scale();
       src_scale[0] = 1.0f / src_scale[0];
       src_attr = {0, src_scale};
     }

     // We intentionally didn't set weight desc to format `any` so DNNL wouldn't
     // have to determine weight format for us. Because the weight tensor from
     // pytorch may have a transposed format (say `ba`). However, DNNL would
     // choose plain format for it by default (`ab` in this case), which would
     // introduces *an extra reorder* afterwards. Here we keep the weight format
     // untouched thanks to optimizations for both plain and transposed formats
     // in DNNL.
     IDEEP_ENFORCE(weights.get_data_type() == data_type::f32 ||
		   weights.get_data_type() == data_type::bf16,
                   "Incorrect data type in weights");
     dst_data_type = src.get_data_type() == data_type::bf16 ?
                     data_type::bf16 : data_type::f32;
     src_desc = src.get_desc().to_type(dst_data_type);
     weights_desc = weights.get_desc().to_type(dst_data_type);
     if (with_bias) {
       IDEEP_ENFORCE(bias.get_data_type() == data_type::f32 ||
		     bias.get_data_type() == data_type::bf16,
                     "Incorrect data type in bias");
       bias_desc = bias.get_desc().to_format_any();
       auto bias_scales = scale_t(1, 1.0 / dst_coeff);
       bias_attr = {utils::tensor_scale_mask(1, false), bias_scales};
     }

     if (attr.has_op_kind(kind::sum)) {
       op_attr = attr_t::fuse_sum(sum_coeff);
     }
     int scale_size = 1;
     op_attr.set_output_scales(utils::op_scale_mask(scale_size),
		               std::vector<float>(1, dst_coeff));
   }

   if (task == PrepareStatic || task == PrepareDynamic) {
     dst_data_type = dst_type == data_type::undef ? dst_data_type : dst_type;
     std::vector<int64_t> dst_strides = (ndims == 3) ?
         std::vector<int64_t>({dst_dims[2]* dst_dims[1], dst_dims[1], 1}) :
         std::vector<int64_t>({dst_dims[1], 1});
     tensor::desc dst_desc = is_dynamic ?
         tensor::desc(dst_dims, dst_data_type, dst_strides) :
         tensor::desc(dst_dims, dst_data_type, tag::any);
     auto key = utils::create_key(
         src_desc,
         weights_desc,
         bias_desc,
         dst_desc,
         op_attr,
         with_bias,
         omp_get_max_threads());
     auto pd = fetch_or_create(key, [&]() {
       if (with_bias) {
         return primitive_desc(
             {src_desc, weights_desc, bias_desc, dst_desc}, op_attr, aengine);
       } else {
         return primitive_desc(
             {src_desc, weights_desc, dst_desc}, op_attr, aengine);
       }
     });
     param.primitive = std::move(super(pd));
     param.pd = std::move(pd);
     param.op_attr = std::move(op_attr);
   }

   param.src_attr = std::move(src_attr);
   param.weights_attr = std::move(weights_attr);
   param.bias_attr = std::move(bias_attr);
   param.dst_scales = std::move(dst_scales_in);
   if (task == ComputeDynamic) {
     param.src_desc = src_desc;
     param.weights_desc = weights_desc;
     param.scales_m = std::move(scales_m);
     param.src_zero_point_m = std::move(src_zero_point_m);
     param.wei_zero_point_m = std::move(wei_zero_point_m);
     param.dst_zero_point_m = std::move(dst_zero_point_m);
   }
 }

 static void do_compute(const matmul_forward_params& param,
                        TaskType task,
                        const tensor& src,
                        const tensor& weights,
                        const tensor& bias,
                        bool with_bias,
                        tensor& dst) {
   auto& pd = param.pd;
   auto& primitive = param.primitive;
   auto& op_attr = param.op_attr;
   auto& src_attr = param.src_attr;
   auto& weights_attr = param.weights_attr;
   auto& bias_attr = param.bias_attr;
   auto& dst_scales_in = param.dst_scales;
   auto& src_desc = param.src_desc;
   auto& wei_desc = param.weights_desc;
   auto& scales_m = param.scales_m;
   auto& src_zero_point_m = param.src_zero_point_m;
   auto& wei_zero_point_m = param.wei_zero_point_m;
   auto& dst_zero_point_m = param.dst_zero_point_m;

   auto expected_src_desc = (task == ComputeDynamic) ? src_desc : tensor::desc(pd.src_desc());
   auto expected_wei_desc = (task == ComputeDynamic) ? wei_desc : tensor::desc(pd.weights_desc());
   auto expected_dst_desc = (task == ComputeDynamic) ? dst.get_desc() : pd.dst_desc();
   auto expected_src = src.reorder_if_differ_in(expected_src_desc, src_attr);
   auto expected_weights = weights.reorder_if_differ_in(expected_wei_desc, weights_attr);
   tensor expected_dst;
   if (dst.is_empty() || dst.get_desc() != expected_dst_desc){
     // If dst buffer are not given by user or user given dst buffer are not under expected format
     // We need init a new one
     expected_dst.init(expected_dst_desc);
     if (!dst.is_empty() && op_attr.has_op_kind(kind::sum)) {
       // We need copy the content of given buffer if matmul is fused with sum
       expected_dst.feed_from(dst);
     }
   } else {
     // The format of given dst buffer is expected
     expected_dst = dst;
   }

   if (!dst_scales_in.empty() && utils::one_of(dst.get_data_type(), data_type::u8, data_type::s8)) {
     expected_dst.set_scale(dst_scales_in);
   }
   if (with_bias){
     auto expected_bias = bias.reorder_if_differ_in(pd.bias_desc(), bias_attr);
     primitive.execute(stream::default_stream(),
                       {{DNNL_ARG_SRC, expected_src},
                        {DNNL_ARG_WEIGHTS, expected_weights},
                        {DNNL_ARG_BIAS, expected_bias},
                        {DNNL_ARG_DST, expected_dst},
                        {DNNL_ARG_ATTR_OUTPUT_SCALES, scales_m},
                        {DNNL_ARG_ATTR_ZERO_POINTS | DNNL_ARG_SRC, src_zero_point_m},
                        {DNNL_ARG_ATTR_ZERO_POINTS | DNNL_ARG_WEIGHTS, wei_zero_point_m},
                        {DNNL_ARG_ATTR_ZERO_POINTS | DNNL_ARG_DST, dst_zero_point_m}});
   } else {
     primitive.execute(stream::default_stream(),
                       {{DNNL_ARG_SRC, expected_src},
                        {DNNL_ARG_WEIGHTS, expected_weights},
                        {DNNL_ARG_DST, expected_dst},
                        {DNNL_ARG_ATTR_OUTPUT_SCALES, scales_m},
                        {DNNL_ARG_ATTR_ZERO_POINTS | DNNL_ARG_SRC, src_zero_point_m},
                        {DNNL_ARG_ATTR_ZERO_POINTS | DNNL_ARG_WEIGHTS, wei_zero_point_m},
                        {DNNL_ARG_ATTR_ZERO_POINTS | DNNL_ARG_DST, dst_zero_point_m}});
   }
   // reorder back to dst's buffer if needed
   if (dst.is_empty() ||
         dst.get_desc() == expected_dst.get_desc() ||
         !dst.get_desc().has_same_shape_as(expected_dst.get_desc())){
     dst =  expected_dst;
   } else {
     dst.feed_from(expected_dst);
   }
  }

};

}  // namespace ideep

#endif
