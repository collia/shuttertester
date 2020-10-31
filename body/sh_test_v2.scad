include <threads.scad>

x = 70;
y = 62;
z = 25;

rounding = 3;

module box() {
    minkowski() {
        sphere(rounding);
        cube([x-2*rounding,y-2*rounding,z-2*rounding], center = true);
    }
}

board_x = 32;
board_y = 55;
board_z = 3;

board_1_width = 17;
board_1_length = 55;
board_1_heigth = 17;
usb_x = 0;
usb_y = -board_1_length/2;
usb_z = (board_1_width/2 - 3);

usb_w = 6;
usb_h = 3;
usb_l = 8;
hole_d = 3.5;
hole_x_offset = 12;
hole_y_offset = 25;

module plate() {
    translate([0,0,-board_z/2])
    difference() {
        union() {
            cube([board_1_width, board_1_length, board_1_heigth], center = true);
            translate([usb_x, usb_y, usb_z])
                cube([usb_w, usb_l, usb_h] , center=true);
            translate([0,0,(board_1_heigth+board_z)/2])
                cube([board_x, board_y, board_z], center=true);
        }/*
        translate([hole_x_offset, hole_y_offset, board_z/2])
            cylinder(d=hole_d, h=board_1_heigth+board_z, center=true);
        translate([hole_x_offset, -hole_y_offset, board_z/2])
            cylinder(d=hole_d, h=board_1_heigth+board_z, center=true);
        translate([-hole_x_offset, hole_y_offset, board_z/2])
            cylinder(d=hole_d, h=board_1_heigth+board_z, center=true);
        translate([-hole_x_offset, -hole_y_offset, board_z/2])
            cylinder(d=hole_d, h=board_1_heigth+board_z, center=true);*/
    }
}

cup_border = 4;
cup_heigth = 4;
module cup() {
    translate([0, 0, z/2])
        union() {
            translate([0,0, -cup_heigth/4])
                cube([board_x + cup_border, board_y+cup_border, cup_heigth/2], center=true);
            translate([0,0, -cup_heigth/2])
                cube([board_x + cup_border/2, board_y+cup_border/2, cup_heigth/2], center=true);
        }
}

module nut_hole_3(h) {
    d = 3;
    union() {
        cylinder(d=d, h=h, center=true);
        translate([0,0,h/2])
            cylinder(d1=d, d2=2*d, h=d, center=true);
        //translate([0,0,-h/2])
            //metric_thread (diameter=d, pitch=0.5, length=h, internal=true, n_starts=1);

    }
}
module nuts() {
    translate([hole_x_offset, hole_y_offset, 0])
        nut_hole_3(z);
    translate([-hole_x_offset, hole_y_offset, 0])
        nut_hole_3(z);
    translate([-hole_x_offset, -hole_y_offset, 0])
        nut_hole_3(z);
    translate([hole_x_offset, -hole_y_offset, 0])
        nut_hole_3(z);
}

module body_print() {
    $fn = 20;
    difference() {
        box();
        plate();
        cup();
        nuts();
    }
}

module cup_print() {
    $fn = 20;
    difference() {
        intersection() {
            box();
            cup();
        }
        nuts();
    }

}

body_print();
cup_print();