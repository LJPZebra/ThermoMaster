function [durations, temperatures] = parse(protocol_path)
% Protocols.parse. This function reads a protocol file for ThermoMaster and fetch left/right
% target temperatures and the duration for each temperature conditions.
% Supports only two digits temperatures, with N different temperature
% gradients.
%
% INPUTS :
% ------
% protocol_path : path to the protocol file.
%
% OUTPUTS :
% -------
% timings : Nx1 vector containing wait time after a temperature target is
% set, in seconds.
% temperatures : Nx2 array containing left temperature target on first
% column and right temperature target on right column.

fid = fopen(protocol_path);

c_1 = 0;  % initialise line counter
c_2 = 0;

while 1
    read_string = fgetl(fid);                   % read line by line
    
    if read_string == -1 % end of file
        break
    end
    if contains(read_string, 'targets:')
        c_1 = c_1 + 1;
        temperatures(c_1, :) = [str2double(read_string(9:10)) str2double(read_string(12:end))];
    end
    
    if contains(read_string, 'wait:')
        c_2 = c_2 + 1;
        t(c_2, :) = str2double(read_string(6:end));
    end
    
end

durations = t*1e-3;

fclose(fid);
end