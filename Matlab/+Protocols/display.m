function [] = display(protocol_path)

% 2018-03-28 Protocols.display. Plot protocols for ThermoMaster

[t, T] = Protocols.parseFull(protocol_path);
T_left = T(:, 1);
T_right = T(:, 2);

figure;
hold on;
plot(t, T_left, 'LineWidth', 3);
plot(t, T_right, 'LineWidth', 3);
legend('T_{left}', 'T_{right}');
xlabel('Time [s]');
ylabel('Temperature [°C]');