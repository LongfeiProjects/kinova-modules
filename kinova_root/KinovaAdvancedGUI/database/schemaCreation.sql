create table Trajectory(
   id INTEGER PRIMARY KEY AUTOINCREMENT,
   name CHAR(30) not null,
   description TEXT);


create table CartesianInfo(
   id INTEGER Primary key autoincrement,
   pos_x real not null,
   pos_y real not null,
   pos_z real not null,
   theta_x real not null,
   theta_y real not null,
   theta_z real not null,
   finger1 real not null,
   finger2 real not null,
   finger3 real not null,	
   angvel_j1 real not null,
   angvel_j2 real not null,
   angvel_j3 real not null,
   angvel_j4 real not null,
   angvel_j5 real not null,
   angvel_j6 real not null);

create table trajectory_cartesianInfo(
trajectory_id int not null,
cartesianInfo_id int not null,
seq int not null,
timestamp int,
foreign key(trajectory_id) references Trajectory(id),
foreign key(cartesianInfo_id) references CartesianInfo(id));
