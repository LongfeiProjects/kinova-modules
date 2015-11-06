create table Trajectory(
   id INTEGER PRIMARY KEY AUTOINCREMENT,
   name CHAR(30) not null,
   description TEXT);


create table CartesianInfo(
   id INTEGER Primary key autoincrement,
   pos_x real not null,
   pos_y real not null,
   pos_z real not null,
   vel_x real not null,
   vel_y real not null,
   vel_z real not null);

create table trajectory_cartesianInfo(
trajectory_id int not null,
cartesianInfo_id int not null,
seq int not null,
timestamp real,
foreign key(trajectory_id) references Trajectory(id),
foreign key(cartesianInfo_id) references CartesianInfo(id));
