% 2018-05-22 - Create uniform protocol for ThermoMaster.

clear

output_dir = '/home/ljp/Science/Projects/ThermoMaster/Data/Protocols/Uniform/';

% Temperatures parameters
% -----------------------
T_baseline = 28;        % the baseline temperature
n_condition = 2;        % number of conditions
dT_max = 12;            % maximum temperature from 28

% Timings parameters
% ------------------
t_baseline = 10;        % time spent in baseline (min)
t_gradient = 20;        % time spent in gradient (min)

% Pick gradients and sides
% ------------------------
dT = randi([-dT_max dT_max], 1, n_condition);

% Commands definition
% -------------------
cam_start = 'camera:start\n';                                       % start recording images
cam_stop = 'camera:stop\n';                                         % stop recording images
create_dir = 'data:create directory\n';                             % create Run directory
set_T = @(Tl, Tr) ['targets:' num2str(Tl) ':' num2str(Tr) '\n'];    % set left and right targets
wait = @(wait_time) ['wait:' num2str(wait_time*60*1000) '\n'];      % wait wait_time minutes

% Create targets
% --------------
targets = cell(n_condition, 1);
for idx = 1:n_condition
    
    targets{idx} = set_T(T_baseline + dT(idx), T_baseline + dT(idx));

end

% Create filename
% ---------------
filename = ['baseline=' num2str(T_baseline) '_uniform_' num2str(t_baseline) 'min_dT=' ...
    num2str(dT) '_' num2str(t_gradient) 'min.protocol'];

% Write protocol
% --------------
fid = fopen([output_dir filename], 'w');

% Header
fprintf(fid, '# Protocol : %s\n', filename);

% Verbose
fprintf(fid, '\print:Starting protocol...\n');
% Create directory
fprintf(fid, create_dir);
% Comment
fprintf(fid, '\Start images recording\n');
% Start camera
fprintf(fid, cam_start);

% Write orders

for c = 1:n_condition
    fprintf(fid, '\n# Set baseline condition\n');
    fprintf(fid, set_T(T_baseline, T_baseline));
    fprintf(fid, '\n# Wait\n');
    fprintf(fid, wait(t_baseline));
    
    fprintf(fid, '\n# Set gradient condition\n');
    fprintf(fid, targets{c});
    fprintf(fid, '\n# Wait\n');
    fprintf(fid, wait(t_gradient));
end

% Return to baseline and finish experiment
fprintf(fid, set_T(T_baseline, T_baseline));
fprintf(fid, '\n# Stop images recording\n');
fprintf(fid, cam_stop);
fprintf(fid, 'print:Protocol ended');

fclose(fid);

Protocols.display([output_dir filename]);