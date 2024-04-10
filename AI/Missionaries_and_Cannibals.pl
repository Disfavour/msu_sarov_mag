% H - human, C - cannibal, B - boat (0 - left, 1 - right), S - список состояний (closed), R - result
valid_base(H, C):- H >= 0, C >= 0, H < 4, C < 4.

% проверка состояния на этом берегу
valid(H, C):- valid_base(H, C), H >= C, H1 is 3 - H, C1 is 3 - C, valid_another(H1, C1).
valid(H, C):- valid_base(H, C), H == 0.

% проверка состояния на другом берегу
valid_another(H, C):- valid_base(H, C), H >= C.
valid_another(H, C):- valid_base(H, C), H == 0.

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
