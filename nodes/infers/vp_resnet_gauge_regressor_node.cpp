
#include <algorithm>
#include <cmath>

#include "vp_resnet_gauge_regressor_node.h"

namespace vp_nodes {
    
    vp_resnet_gauge_regressor_node::vp_resnet_gauge_regressor_node(std::string node_name, 
                            std::string model_path, 
                            int input_width, 
                            int input_height,
                            std::vector<int> p_class_ids_applied_to,
                            int min_width_applied_to,
                            int min_height_applied_to,
                            float pad_ratio,
                            float scale,
                            cv::Scalar mean,
                            cv::Scalar std,
                            bool swap_rb):
                            vp_secondary_infer_node(node_name, model_path, "", "", input_width, input_height, 1, 
                                                    p_class_ids_applied_to, min_width_applied_to, min_height_applied_to, 0,
                                                    scale, mean, std, swap_rb),
                            pad_ratio(pad_ratio) {
        this->initialized();
    }
    
    vp_resnet_gauge_regressor_node::~vp_resnet_gauge_regressor_node() {
        deinitialized();
    }

    void vp_resnet_gauge_regressor_node::prepare(const std::vector<std::shared_ptr<vp_objects::vp_frame_meta>>& frame_meta_with_batch, std::vector<cv::Mat>& mats_to_infer) {
        assert(frame_meta_with_batch.size() == 1);

        target_indices.clear();

        auto& frame_meta = frame_meta_with_batch[0];

        for (int i = 0; i < frame_meta->targets.size(); i++) {
            auto& target = frame_meta->targets[i];

            if (!need_apply(target->primary_class_id, target->width, target->height)) {
                continue;
            }

            // keep a square-ish context around the gauge to preserve geometry
            auto side = std::max(1.0f, (float)std::max(target->width, target->height));
            side = side * (1.0f + 2.0f * std::max(0.0f, pad_ratio));

            auto cx = target->x + target->width / 2.0f;
            auto cy = target->y + target->height / 2.0f;

            auto x1 = (int)std::round(cx - side / 2.0f);
            auto y1 = (int)std::round(cy - side / 2.0f);
            auto x2 = (int)std::round(cx + side / 2.0f);
            auto y2 = (int)std::round(cy + side / 2.0f);

            // clip to frame
            x1 = std::max(x1, 0);
            y1 = std::max(y1, 0);
            x2 = std::min(x2, frame_meta->frame.cols);
            y2 = std::min(y2, frame_meta->frame.rows);

            if (x2 <= x1 || y2 <= y1) {
                continue;
            }

            target_indices.push_back(i);
            mats_to_infer.push_back(frame_meta->frame(cv::Rect(x1, y1, x2 - x1, y2 - y1)));
        }
    }

    void vp_resnet_gauge_regressor_node::postprocess(const std::vector<cv::Mat>& raw_outputs, const std::vector<std::shared_ptr<vp_objects::vp_frame_meta>>& frame_meta_with_batch) {
        assert(raw_outputs.size() > 0);
        assert(frame_meta_with_batch.size() == 1);

        auto& frame_meta = frame_meta_with_batch[0];
        auto& output = raw_outputs[0];
        auto count = (int)output.total();

        if (count == 0) {
            return;
        }

        // outputs are in the same order as the crops prepared above
        auto data = output.ptr<float>(0);

        for (int i = 0; i < (int)target_indices.size() && i < count; i++) {
            auto& target = frame_meta->targets[target_indices[i]];
            auto value = data[i];

            // update back to frame meta
            target->gauge_value = value;
            target->secondary_scores.push_back(value);
            target->secondary_labels.push_back(vp_utils::string_format("%.3f", value));
        }
    }
}
