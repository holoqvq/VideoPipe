
#pragma once

#include "../vp_node.h"

namespace vp_nodes {
    // gauge osd, draws gauge rect and reading value, see vp_osd_node_v2 also.
    class vp_gauge_value_osd_node final: public vp_node
    {
    protected:
        virtual std::shared_ptr<vp_objects::vp_meta> handle_frame_meta(std::shared_ptr<vp_objects::vp_frame_meta> meta) override;
    public:
        vp_gauge_value_osd_node(std::string node_name);
        ~vp_gauge_value_osd_node();
    };
}
