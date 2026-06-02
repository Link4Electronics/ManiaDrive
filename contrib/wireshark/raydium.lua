-- WIP !!!
-- Very VERY experimental Wireshark lua dissectors for Raydium 3D engine applications and servers
-- Xfennec,
-- http://raydium.org

-- You must enable lua in Wireshark (see init.lua) and call this script:
-- -X lua_script:.../raydium.lua, or dofile ".../raydium.lua" in the user init.lua)

-- "defines"
PACKET_OFFSET = 4

-- declare our protocol
raydium_proto = Proto("raydium","Raydium Network Protocol")

-- create a function to dissect it
function raydium_proto.dissector(buffer,pinfo,tree)
    pinfo.cols.protocol = "RAYDIUM"

    local type = buffer(0,1):uint()
    local tcpid=buffer(2,2):le_uint()

    local data_str="(no or unknown data)";
    local data_len=1
    local display_from=true

    if type==1 then
        type_str="DATA"
    elseif type==2 then
        type_str="ERROR_NO_MORE_PLACE"
    elseif type==3 then
        type_str="ATTRIB_UID"
        local id=buffer(PACKET_OFFSET,1):uint()
        data_str="ID: " .. id
        display_from=false
    elseif type==4 then
        type_str="PACKET_REQUEST_UID"
        local name=buffer(PACKET_OFFSET):string()
        data_str="NAME: " .. name
        data_len=string.len(name)+1
        display_from=false
    elseif type==5 then
        type_str="INFO_NAME"
        local id=buffer(PACKET_OFFSET,1):uint()
        local name=buffer(PACKET_OFFSET+1):string()
        data_str="NAME: " .. name .. " is ID " .. id
        data_len=string.len(name)+2
        display_from=false
    elseif type==6 then
        type_str="ACK"
        data_str="ACK for " .. buffer(PACKET_OFFSET,2):le_uint()
        data_len=2
    elseif type==7 then
        type_str="SERVER_BEACON"
    elseif type==10 then
        type_str="ODE_DATA"
        local n=buffer(PACKET_OFFSET,2):le_uint()
        data_str=n .. " element(s) refreshed"
        data_len=2+n*(4 + 3*4 + 4*4 + 3*4);
    elseif type==11 then
        type_str="ODE_NEWELEM"
        local dec=0
        local nid=buffer(PACKET_OFFSET+dec,4):le_uint()
        dec=dec+4
        local shape=buffer(PACKET_OFFSET+dec,4):le_uint()
        dec=dec+4
        if shape==0 then
            shape_str="dSphereClass"
        elseif shape==1 then
            shape_str="dBoxClass"
        elseif shape==2 then
            shape_str="dCapsuleClass"
        else
            shape_str="unknown_class[" .. shape .. "]"
        end
        local s1=buffer(PACKET_OFFSET+dec,4):le_float() -- ?
        dec=dec+4
        local s2=buffer(PACKET_OFFSET+dec,4):le_float() -- ?
        dec=dec+4
        local s3=buffer(PACKET_OFFSET+dec,4):le_float() -- ?
        dec=dec+4
        shape_str=shape_str .. "(" .. s1 .. "," .. s2 .. "," .. s3 ..")"

        local tag=buffer(PACKET_OFFSET+dec,4):le_uint()
        dec=dec+4

        local mesh=buffer(PACKET_OFFSET+dec):string()
        data_str="'" .. mesh .. "'" .. " " .. shape_str .. " with NID=" .. nid .. " and user_tag=" .. tag
        data_len=dec + string.len(mesh) + 1
    elseif type==12 then
        type_str="ODE_REMELEM"
        local nid=buffer(PACKET_OFFSET,4):le_uint()
        data_str="Delete NID=" .. nid
    elseif type==13 then
        type_str="ODE_NIDWHO"
        local nid=buffer(PACKET_OFFSET,4):le_uint()
        data_str="Who is NID=" .. nid
        data_len=4
    elseif type==14 then
        type_str="ODE_EXPLOSION"
    elseif type>=20 then
        type_str="USER (" .. type .. ")"
    else
        type_str="(unkown type)"
    end


    if pinfo.src_port==29104 then where="FROM server" end
    if pinfo.dst_port==29104 then where="TO server" end

    local subtree = tree:add(raydium_proto,buffer(),"Raydium Network Protocol")
    subtree:add(buffer(0,1),"Packet type: " .. type_str .. " (" .. where .. ")")
    if display_from then subtree:add(buffer(1,1),"From ID: " .. buffer(1,1):uint() .. " (may be uninitialized)") end
    if tcpid~=0 then subtree:add(buffer(2,2),"TCPID: " .. tcpid) end
    subtree:add(buffer(PACKET_OFFSET,data_len),data_str)

-- For future reference ...
--    subtree = subtree:add(buffer(2,2),"The next two bytes")
--    subtree:add(buffer(2,1),"The 3rd byte: " .. buffer(2,1):uint())
end

-- load the udp.port table
udp_table = DissectorTable.get("udp.port")
udp_table:add(29104,raydium_proto)
