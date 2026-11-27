
#pragma once

#include "../vp_secondary_infer_node.h"

namespace vp_nodes {
    // gauge reading regressor based on resnet, see vp_classifier_node also.
    // infers on cropped gauge ROI and updates gauge_value of vp_frame_target.
    class vp_resnet_gauge_regressor_node final: public vp_secondary_infer_node
    {
    private:
        // padding ratio applied on the longer side of the detection box
        float pad_ratio;
        // target indices recorded by prepare()
        std::vector<int> target_indices;
    protected:
        virtual void prepare(const std::vector<std::shared_ptr<vp_objects::vp_frame_meta>>& frame_meta_with_batch, std::vector<cv::Mat>& mats_to_infer) override;
        virtual void postprocess(const std::vector<cv::Mat>& raw_outputs, const std::vector<std::shared_ptr<vp_objects::vp_frame_meta>>& frame_meta_with_batch) override;
    public:
        vp_resnet_gauge_regressor_node(std::string node_name, 
                            std::string model_path, 
                            int input_width = 256, 
                            int input_height = 256,
                            std::vector<int> p_class_ids_applied_to = std::vector<int>(),
                            int min_width_applied_to = 0,
                            int min_height_applied_to = 0,
                            float pad_ratio = 0.08,
                            float scale = 1 / 255.0,
                            cv::Scalar mean = cv::Scalar(123.675, 116.28, 103.53),  // imagenet dataset
                            cv::Scalar std = cv::Scalar(1),
                            bool swap_rb = true);
        ~vp_resnet_gauge_regressor_node();
    };
}
