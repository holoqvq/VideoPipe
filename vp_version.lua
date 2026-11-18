local function git_output(command)
    local pipe = io.popen(command .. " 2>NUL")
    local result = pipe:read("*a")
    pipe:close()
    return result:gsub("%s+", "")
end

local build_time = os.date("%Y%m%d-%H%M%S")

local commit = git_output("git rev-parse --short HEAD")

if git_output("git status --porcelain") ~= "" then
    commit = commit .. "-dirty"
end

local file = assert(io.open("utils/vp_version.h.in", "r"))
local content = file:read("*a")
file:close()

content = content:gsub("@BUILD_TIME@", build_time)
content = content:gsub("@FINAL_GIT_VERSION@", commit)

file = assert(io.open("utils/vp_version.h", "w"))
file:write(content)
file:close()

print("Generated utils/vp_version.h")
