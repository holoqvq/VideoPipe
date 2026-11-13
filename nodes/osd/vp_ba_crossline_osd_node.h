#pragma once

#include <map>
#include "../vp_node.h"
#include "../../objects/shapes/vp_point.h"
#include "../../objects/shapes/vp_line.h"

namespace vp_nodes {
    // osd node for behaviour analysis of crossline
    class vp_ba_crossline_osd_node: public vp_node
    {
    private:
        // support multi channels
        std::map<int, int> all_total_crossline;
        std::map<int, vp_objects::vp_line> all_lines;
    protected:
        virtual std::shared_ptr<vp_objects::vp_meta> handle_frame_meta(std::shared_ptr<vp_objects::vp_frame_meta> meta) override;
    public:
        vp_ba_crossline_osd_node(std::string node_name);
        ~vp_ba_crossline_osd_node();
    };
}