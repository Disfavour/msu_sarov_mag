% возможные состояния с учетом того, что людей должно быть больше чем людоедов на каждом берегу
valid(3, 3).
valid(3, 2).
valid(3, 1).
valid(3, 0).
valid(2, 2).
valid(1, 1).
valid(0, 0).
valid(0, 3).
valid(0, 2).
valid(0, 1).

% H - human
% C - cannibal
% B - boat (0 - left, 1 - right)
% S - список состояний (closed)
state(0, 0, 1, _).

% плывем на правый берег
state(H, C, 0, S):- H1 is H - 2, C1 is C - 0, valid(H1, C1), not(member([H1, C1, 1], S)), writeln([[H, C, 0]|S]), state(H1, C1, 1, [[H, C, 0]|S]).
state(H, C, 0, S):- H1 is H - 1, C1 is C - 1, valid(H1, C1), not(member([H1, C1, 1], S)), writeln([[H, C, 0]|S]), state(H1, C1, 1, [[H, C, 0]|S]).
state(H, C, 0, S):- H1 is H - 0, C1 is C - 2, valid(H1, C1), not(member([H1, C1, 1], S)), writeln([[H, C, 0]|S]), state(H1, C1, 1, [[H, C, 0]|S]).
state(H, C, 0, S):- H1 is H - 1, C1 is C - 0, valid(H1, C1), not(member([H1, C1, 1], S)), writeln([[H, C, 0]|S]), state(H1, C1, 1, [[H, C, 0]|S]).
state(H, C, 0, S):- H1 is H - 0, C1 is C - 1, valid(H1, C1), not(member([H1, C1, 1], S)), writeln([[H, C, 0]|S]), state(H1, C1, 1, [[H, C, 0]|S]).

% плывем на левый берег
state(H, C, 1, S):- H1 is H + 2, C1 is C + 0, valid(H1, C1), not(member([H1, C1, 0], S)), writeln([[H, C, 1]|S]), state(H1, C1, 0, [[H, C, 1]|S]).
state(H, C, 1, S):- H1 is H + 1, C1 is C + 1, valid(H1, C1), not(member([H1, C1, 0], S)), writeln([[H, C, 1]|S]), state(H1, C1, 0, [[H, C, 1]|S]).
state(H, C, 1, S):- H1 is H + 0, C1 is C + 2, valid(H1, C1), not(member([H1, C1, 0], S)), writeln([[H, C, 1]|S]), state(H1, C1, 0, [[H, C, 1]|S]).
state(H, C, 1, S):- H1 is H + 1, C1 is C + 0, valid(H1, C1), not(member([H1, C1, 0], S)), writeln([[H, C, 1]|S]), state(H1, C1, 0, [[H, C, 1]|S]).
state(H, C, 1, S):- H1 is H + 0, C1 is C + 1, valid(H1, C1), not(member([H1, C1, 0], S)), writeln([[H, C, 1]|S]), state(H1, C1, 0, [[H, C, 1]|S]).

% ?-state(3, 3, 0, []).
