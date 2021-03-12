clc

% === Parameters ==========================================================

DataDir = '/home/ljp/Science/Projects/ThermoMaster/Data/Calibration/';

fname = [DataDir 'Lc_200_1.txt'];

% =========================================================================

% --- Open serial connection
s = serial('/dev/ttyACM2'); 		% Replace with your actual serial port
set(s, 'BaudRate', 115200);
set(s, 'TimeOut', 0.01);
fopen(s);
fprintf('The serial connection is established.\n');

pause(5)
tic

Data = NaN(0,6);

% Display
last = 0;

% Receive data
 while true
     
     Data(end+1,:) = cellfun(@str2double, strsplit(strtrim(fscanf(s))));
     
     t = toc;
     
     if t>last+1
         
        Times = (Data(:,1) - Data(1))*1e-6;

        clf
        hold on
        box on
        
        yyaxis left
        plot(Times, Data(:,2), 'b-');
        plot(Times, Data(:,3), 'm-');
        
        plot(Times, Data(:,4), 'g-');
        plot(Times, Data(:,5), 'y-');
                
        yyaxis right
        plot(Times, Data(:,6), 'k-');
        
        drawnow
        
        last = t;
     
     end
     
     if t>150
         break;
     end
   
     % if ~s.BytesAvailable, break; end
 end

% --- Close the serial connection
fclose(s);
delete(s);
clear s

fprintf('The serial connection is closed.\n');

% --- Save Data

dlmwrite(fname, Data);

% --- Display 

legend({'T_{Left}' ; 'T_{Right}' ; 'T_{Left dye}' ; 'T_{Right dye}'});

