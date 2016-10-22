-- Returns an array of (x,y) offsets the game should try when
-- there's a collision using a piece's default rotated position.
--
-- piece: a letter, one of I,J,L,O,S,T,Z
-- start: the orientation of the piece before the rotation, one of N,S,W,E
-- clockwise: true if the piece should be rotated clockwise, false otherwise
function possibleRotations(piece, start, clockwise)
    output = {}

    if piece == 'O' then
        return output
    end

    if piece == 'I' then
        if start == 'N' then
            if clockwise then
                table.insert(output, {-2, 0})
                table.insert(output, {1, 0})
                table.insert(output, {-2, 1})
                table.insert(output, {1, -2})
            else
                table.insert(output, {-1, 0})
                table.insert(output, {2, 0})
                table.insert(output, {-1, -2})
                table.insert(output, {2, 1})
            end
        end
        if start == 'E' then
            if clockwise then
                table.insert(output, {-1, 0})
                table.insert(output, {2, 0})
                table.insert(output, {-1, -2})
                table.insert(output, {2, 2})
            else
                table.insert(output, {2, 0})
                table.insert(output, {-1, 0})
                table.insert(output, {2, -1})
                table.insert(output, {-1, 2})
            end
        end
        if start == 'S' then
            if clockwise then
                table.insert(output, {2, 0})
                table.insert(output, {-1, 0})
                table.insert(output, {2, -1})
                table.insert(output, {-1, 2})
            else
                table.insert(output, {1, 0})
                table.insert(output, {-2, 0})
                table.insert(output, {1, 2})
                table.insert(output, {-2, -1})
            end
        end
        if start == 'W' then
            if clockwise then
                table.insert(output, {1, 0})
                table.insert(output, {-2, 0})
                table.insert(output, {1, 2})
                table.insert(output, {-2, -1})
            else
                table.insert(output, {-2, 0})
                table.insert(output, {1, 0})
                table.insert(output, {-2, 1})
                table.insert(output, {1, -2})
            end
        end
    else
        if start == 'N' then
            if clockwise then
                table.insert(output, {-1, 0})
                table.insert(output, {-1, -1})
                if piece ~= 'T' then table.insert(output, {0, 2}) end
                table.insert(output, {-1, 2})
            else
                table.insert(output, {1, 0})
                table.insert(output, {1, -1})
                if piece ~= 'T' then table.insert(output, {0, 2}) end
                table.insert(output, {1, 2})
            end
        end
        if start == 'E' then
            table.insert(output, {1, 0})
            table.insert(output, {1, 1})
            table.insert(output, {0, -2})
            table.insert(output, {1, -2})
        end
        if start == 'S' then
            if clockwise then
                table.insert(output, {1, 0})
                if piece ~= 'T' then table.insert(output, {1, -1}) end
                table.insert(output, {0, 2})
                table.insert(output, {1, 2})
            else
                table.insert(output, {-1, 0})
                if piece ~= 'T' then table.insert(output, {-1, -1}) end
                table.insert(output, {0, 2})
                table.insert(output, {-1, 2})
            end
        end
        if start == 'W' then
            table.insert(output, {-1, 0})
            table.insert(output, {-1, 1})
            table.insert(output, {0, -2})
            table.insert(output, {-1, -2})
        end
    end

    return output
end
