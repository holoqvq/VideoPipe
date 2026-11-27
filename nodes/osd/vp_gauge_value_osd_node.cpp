
#include <opencv2/imgproc.hpp>

#include "vp_gauge_value_osd_node.h"

namespace vp_nodes {
    
    vp_gauge_value_osd_node::vp_gauge_value_osd_node(std::string node_name): vp_node(node_name) {
        this->initialized();
    }
    
    vp_gauge_value_osd_node::~vp_gauge_value_osd_node() {
        deinitialized();
    }

    std::shared_ptr<vp_objects::vp_meta> vp_gauge_value_osd_node::handle_frame_meta(std::shared_ptr<vp_objects::vp_frame_meta> meta) {
        if (meta->osd_frame.empty()) {
            meta->osd_frame = meta->frame.clone();
        }
        auto& canvas = meta->osd_frame;

        // scan targets
        for (auto& target : meta->targets) {
            cv::rectangle(canvas, cv::Rect(target->x, target->y, target->width, target->height), cv::Scalar(0, 255, 0), 2);

            if (target->gauge_value >= 0) {
                auto text = vp_utils::string_format("reading: %.3f", target->gauge_value);
                cv::putText(canvas, text, cv::Point(target->x, std::max(15, target->y - 8)),
                            cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);
            }
        }

        return meta;
    }
}
