clc

% === Parameters ==========================================================

DataDir = '/home/ljp/Science/Projects/Behavior/ThermoMaster/Data/Calibration/';

Dur = [10000 1000 500 200];

clf
hold on
cmL = bone(numel(Dur)+1);

res = NaN(1, numel(Dur));

for i = 1:numel(Dur)

    % --- Get Data
    Data = dlmread([DataDir 'Lc_' num2str(Dur(i)) '_1.txt']);
   
    t = (Data(:,1) - Data(1,1))*1e-6;
    TL = Data(:,2);
    TR = Data(:,3);
    Stim = Data(:,6);
    
    % --- Process signals
    
    L = TL - mean(TL(1:50));
    
    [~, k1] = min((t-7.5 - Dur(i)/1000).^2);
    [~, k2] = min((t-7.5 - Dur(i)/1000).^2);
    
    res(i) = mean(L(k1:k2));
    
end

plot(Dur, res, '+-')

% for i = 1:numel(Dur)
% 
%     % --- Get Data
%     Data = dlmread([DataDir 'Rh_' num2str(Dur(i)) '_1.txt']);
%    
%     t = (Data(:,1) - Data(1,1))*1e-6;
%     TL = Data(:,2);
%     TR = Data(:,3);
%     Stim = Data(:,6);
%     
%     % --- Process signals
%     
%     R = TR - mean(TR(1:50));
%     
%     [~, k1] = min((t-7.5 - Dur(i)/1000).^2);
%     [~, k2] = min((t-7.5 - Dur(i)/1000).^2);
%     
%     res(i) = mean(R(k1:k2));
%     
% end
% 
% plot(Dur, res, '+-')

box on
