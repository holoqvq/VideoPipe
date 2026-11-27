-- Generates utils/vp_version.h from utils/vp_version.h.in.
-- Works on Windows (cmd.exe) and Linux / macOS (POSIX sh).

-- stderr sink differs per platform: NUL on cmd.exe, /dev/null on POSIX sh.
local is_windows = package.config:sub(1, 1) == "\\"
local null_device = is_windows and "NUL" or "/dev/null"

local function git_output(command)
    local pipe = io.popen(command .. " 2>" .. null_device)
    if not pipe then
        return ""
    end

    local result = pipe:read("*a") or ""
    pipe:close()
    return (result:gsub("%s+", ""))
end

local build_time = os.date("%Y%m%d-%H%M%S")

local commit = git_output("git rev-parse --short HEAD")

-- Not a git checkout (source archive) or git is missing.
if commit == "" then
    commit = "unknown"
end

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
