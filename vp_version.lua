local is_windows = package.config:sub(1, 1) == "\\"
local null_device = is_windows and "NUL" or "/dev/null"

local function git_output(command)
    local pipe = io.popen(command .. " 2>" .. null_device)
    if not pipe then
        return nil
    end

    local result = pipe:read("*a")
    pipe:close()

    return result and result:gsub("%s+", "")
end

-- Build time
local build_time = os.date("%Y%m%d-%H%M%S")

-- Git version
local commit = git_output("git rev-parse --short HEAD") or "nogit"

if commit ~= "nogit" then
    local status = git_output("git status --porcelain")
    if status and status ~= "" then
        commit = commit .. "-dirty"
    end
end

-- Read template
local template_path = "utils/vp_version.h.in"
local output_path = "utils/vp_version.h"

local file = assert(io.open(template_path, "r"))
local content = file:read("*a")
file:close()

-- Replace placeholders
content = content:gsub("@BUILD_TIME@", build_time)
content = content:gsub("@FINAL_GIT_VERSION@", commit)

-- Write header
file = assert(io.open(output_path, "w"))
file:write(content)
file:close()

print("Generated " .. output_path)
