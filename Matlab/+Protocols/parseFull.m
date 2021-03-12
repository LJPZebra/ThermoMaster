function [t, T] = parseFull(protocol_path)
% Protocols.parseFull. This function returns the time and the temperatures vectors of a given
% protocol, according to the protocol file. It uses the protocolParser
% function but the left and right temperatures can be plotted versus time.
%
% INPUTS :
% ------
% protocol_path : absolute path to the protocol file.
%
% OUTPUTS :
% -------
% t : time vector in seconds, contains N seconds.
% T : Nx2 array, first column is the left temperature and the second is the
% right temperature.
%
% 2018-03-28 GLG

[timings, temperatures] = Protocols.parse(protocol_path);

total_duration = sum(timings);

t = 0:total_duration - 1;
T_left = NaN(total_duration, 1);
T_right = NaN(total_duration, 1);
t_stop = 0;

for i = 1:numel(timings)
    
    t_start = t_stop + 1;
    t_stop = t_stop + timings(i);
    
    T_left(t_start:t_stop) = temperatures(i, 1);
    T_right(t_start:t_stop) = temperatures(i ,2);
end

T = [T_left, T_right];