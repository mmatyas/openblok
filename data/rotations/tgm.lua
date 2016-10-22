plugin_name = 'TGM rotation system'

-- Returns an array of (x,y) offsets the game should try when
-- there's a collision using a piece's default rotated position.
--
-- piece: a letter, one of I,J,L,O,S,T,Z
-- start: the orientation of the piece before the rotation, one of N,S,W,E
-- clockwise: true if the piece should be rotated clockwise, false otherwise
function possibleRotations(piece, from, to)
    output = {
        {1, 0}, -- try 1 tile right
        {-1, 0}, -- try 1 tile left
        {1, -1}, -- try 1 tile right, 1 tile up (floor kick)
        {-1, -1} -- try 1 tile left, 1 tile up (floor kick)
    }

    -- for I pieces, also try moving 2 tiles
    if piece == 'I' then
        table.insert(output, {2, 0})
        table.insert(output, {-2, 0})
        table.insert(output, {2, -1})
        table.insert(output, {-2, -1})
        table.insert(output, {2, -2})
        table.insert(output, {-2, -2})
    end

    return output
end
