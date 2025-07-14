#! /usr/bin/lua

-- global
filename = "upgrade.bin"
DEAL_BUF_LEN = 1024*10
STATE_START = 0
STATE_GET_FILE_START = 1
STATE_GET_FILE_CONTENT = 2
STATE_END = 3

logfile = io.open("/tmp/logfile","w")
logfile:write("log start\n")

-- get signcode -- use to judge end
function get_signcode(line)
    local i,j = string.find(line,"\r")
    local signcode = "nil"
    if i ~= nil then
        signcode = string.sub(line,1,i-1)
    end
    logfile:write("signcode is "..signcode.."\n")
    return signcode
end

-- get file name
function get_filename(line)
    local filename = "filename"
    local s,e = string.find(line,"filename=")
    if e and string.sub(line,e+1,e+1) == '\"' then
        e = e + 1
        filename = string.sub(line,e+1)
        s,e = string.find(filename,"\r")
        filename = string.sub(filename,1,s-1)
        filename = string.gsub(filename,"\"","")
    end
    logfile:write("0filename is "..filename.."\n")
    return filename
end

function error_exit(code,msg)
    if uploadfile then
        io.close(uploadfile)
    end
    if logfile then
        io.close(logfile)
    end
    io.write("Status: 500 \r\n")
    io.write("Content-Type: text/html;charset=UTF-8\r\n")
    io.write("\r\n")
    io.write("upload file fail,error code "..tostring(code).." msg:"..msg.."\r\n")
    os.exit()
end

local contentLength = os.getenv("CONTENT_LENGTH")
if contentLength == nil then
    error_exit("-1","can not get CONTENT_LENGTH")
end
contentLength = tonumber(contentLength,10)

local newline
local signcode
local signcodeLen = 0
local s,e
local writedone = 0
local state = STATE_START
local uploadfile
while contentLength > 0 do
    newline = io.read(DEAL_BUF_LEN)
    -- writedone 1 mean write to file done
    writedone = 0
    if newline ~= nil then
        -- contentLength is remain length
        contentLength = contentLength - string.len(newline)
    else
        error_exit("-2","read buff fail")
    end
    -- deal newline
    while writedone ~= 1 do
        if state == STATE_START then
            signcode = get_signcode(newline)
            if signcode ~= "nil" then
                state = STATE_GET_FILE_START
                signcodeLen = string.len(signcode)
            else
                contentLength = contentLength + string.len(newline)
                newline = newline..io.read(DEAL_BUF_LEN)
                contentLength = contentLength - string.len(newline)
            end
        end
        if state == STATE_GET_FILE_START then
            s,e = string.find(newline,"\r\n\r\n")
            if e ~= nil then
                uploadfile  = io.open("/tmp/"..filename,"w")
                logfile:write("e is "..tostring(e).."\n")
                newline = string.sub(newline,e+1)
                state = STATE_GET_FILE_CONTENT
            else
                contentLength = contentLength + string.len(newline)
                newline = newline..io.read(DEAL_BUF_LEN)
                contentLength = contentLength - string.len(newline)
            end
        end
        if state == STATE_GET_FILE_CONTENT then
            if contentLength > (signcodeLen+2) then
                uploadfile:write(newline)
                writedone = 1
            elseif contentLength == 0 then
                state = STATE_END
            else
                newline = newline..io.read("*all")
                contentLength = 0
                state = STATE_END
            end
        end
        if state == STATE_END then
            if signcode ~= "nil" then
                s,e = string.find(newline,signcode.."--")
            else
                error_exit("-3","signcode is nil")
            end
            if s ~= nil then
                newline = string.sub(newline,1,s-1-2)--2 is '\r\n'
                logfile:write("write 2 newline end\n")
            else
                error_exit("-4","can not find signcode-- end")
            end
            uploadfile:write(newline)
            io.close(uploadfile)
            writedone = 1
            break
        end
    end
end

-- write end
io.close(logfile)

io.write("Status: 200 \r\n")
io.write("Content-Type: text/html;charset=UTF-8\r\n")
io.write("\r\n")
io.write("upload file success\r\n")
io.write("<script>\r\n window.top.window.startUpgrade();\r\n</script>\r\n")
