
#include <algorithm>
#include <cmath>

#include "vp_yolo_gauge_detector_node.h"

namespace vp_nodes {
    
    vp_yolo_gauge_detector_node::vp_yolo_gauge_detector_node(std::string node_name, 
                            std::string model_path, 
                            int input_width, 
                            int input_height,
                            int class_id_offset,
                            float score_threshold,
                            float nms_threshold,
                            float scale,
                            cv::Scalar mean,
                            cv::Scalar std,
                            bool swap_rb,
                            std::string gauge_label):
                            vp_primary_infer_node(node_name, model_path, "", "", input_width, input_height, 1, class_id_offset, scale, mean, std, swap_rb),
                            score_threshold(score_threshold),
                            nms_threshold(nms_threshold),
                            gauge_label(gauge_label) {
        this->initialized();
    }
    
    vp_yolo_gauge_detector_node::~vp_yolo_gauge_detector_node() {
        deinitialized();
    }

    int vp_yolo_gauge_detector_node::to_frame_x(float x, bool normalized, int frame_width) {
        return normalized ? int(std::round(x * frame_width)) : int(std::round(x * frame_width / input_width));
    }

    int vp_yolo_gauge_detector_node::to_frame_y(float y, bool normalized, int frame_height) {
        return normalized ? int(std::round(y * frame_height)) : int(std::round(y * frame_height / input_height));
    }

    void vp_yolo_gauge_detector_node::postprocess(const std::vector<cv::Mat>& raw_outputs, const std::vector<std::shared_ptr<vp_objects::vp_frame_meta>>& frame_meta_with_batch) {
        assert(raw_outputs.size() > 0);
        assert(frame_meta_with_batch.size() == 1);

        auto& frame_meta = frame_meta_with_batch[0];
        auto& raw_output = raw_outputs[0];

        // support both `[1, 5, num_anchors]` and `[1, num_anchors, 5]`
        cv::Mat output;
        if (raw_output.dims == 3) {
            cv::Mat tmp(raw_output.size[1], raw_output.size[2], CV_32F, const_cast<float*>(raw_output.ptr<float>(0)));
            if (raw_output.size[1] < raw_output.size[2]) {
                cv::transpose(tmp, output);
            }
            else {
                tmp.copyTo(output);
            }
        }
        else if (raw_output.dims == 2) {
            raw_output.copyTo(output);
        }
        else {
            VP_WARN(vp_utils::string_format("[%s] unexpected output dims: %d", node_name.c_str(), raw_output.dims));
            return;
        }

        if (output.cols < 5) {
            VP_WARN(vp_utils::string_format("[%s] unexpected output shape, cols: %d", node_name.c_str(), output.cols));
            return;
        }

        std::vector<float> confidences;
        std::vector<cv::Rect> boxes;

        // scan anchors
        for (int i = 0; i < output.rows; i++) {
            auto data = output.ptr<float>(i);
            auto score = data[4];
            if (score < score_threshold) {
                continue;
            }

            auto cx = data[0];
            auto cy = data[1];
            auto w = data[2];
            auto h = data[3];

            // absolute coordinates in input space or normalized coordinates
            bool normalized = (w <= 2.0f && h <= 2.0f && cx <= 2.0f && cy <= 2.0f);

            auto left = to_frame_x(cx - w / 2, normalized, frame_meta->frame.cols);
            auto top = to_frame_y(cy - h / 2, normalized, frame_meta->frame.rows);
            auto box_w = to_frame_x(w, normalized, frame_meta->frame.cols);
            auto box_h = to_frame_y(h, normalized, frame_meta->frame.rows);

            boxes.push_back(cv::Rect(left, top, box_w, box_h));
            confidences.push_back(score);
        }

        if (boxes.empty()) {
            return;
        }

        // nms
        std::vector<int> indices;
        cv::dnn::NMSBoxes(boxes, confidences, score_threshold, nms_threshold, indices);

        for (int i = 0; i < indices.size(); ++i) {
            auto idx = indices[i];
            auto box = boxes[idx];
            auto confidence = confidences[idx];

            // check value range
            box.x = std::max(box.x, 0);
            box.y = std::max(box.y, 0);
            box.width = std::min(box.width, frame_meta->frame.cols - box.x);
            box.height = std::min(box.height, frame_meta->frame.rows - box.y);
            if (box.width <= 0 || box.height <= 0) {
                continue;
            }

            auto target = std::make_shared<vp_objects::vp_frame_target>(box.x, box.y, box.width, box.height, 
                                                                    class_id_offset, confidence, frame_meta->frame_index, frame_meta->channel_index, gauge_label);
            frame_meta->targets.push_back(target);
        }
    }
}
