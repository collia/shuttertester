include <threads.scad>

x = 70;
y = 60;
z = 23;

rounding = 3;

module box() {
    minkowski() {
        sphere(rounding);
        cube([x-2*rounding,y-2*rounding,z-2*rounding], center = true);
    }
}

board_x = 50;
board_y = 58;
board_z = 5;

board_1_width = 35;
board_1_length = 59;
board_1_heigth = 15;
usb_x = 5;
usb_y = board_1_length/2;
usb_z = (board_1_heigth/2 - 5.5);

usb_w = 9;
usb_h = 2;
usb_l = 12;
hole_d = 3.5;
hole_x_offset = (39+3)/2;
hole_y_offset = (47+3)/2;

module plate() {
    translate([0,0,-board_z/2])
    difference() {
        union() {
            cube([board_1_width, board_1_length, board_1_heigth], center = true);
            translate([usb_x, usb_y, usb_z])
                cube([usb_w, usb_l, usb_h] , center=true);
            translate([0,0,(board_1_heigth+board_z)/2])
                cube([board_x, board_y, board_z], center=true);
        }
        translate([0, board_1_length/2-3, -(board_1_heigth/2-3)])
            cube([board_1_width, 6, 6], center=true);
        translate([0, -(board_1_length/2-3), -(board_1_heigth/2-3)])
            cube([board_1_width, 6, 6], center=true);
    }
}

cup_border = 4;
cup_heigth = 2;
module cup() {
    translate([0, 0, z/2])
        union() {
            translate([0,0, -cup_heigth/2])
                cube([board_x + cup_border/2, board_y+cup_border/2, cup_heigth], center=true);
        }
}

module nut_hole_3(h) {
    d = 3;
    union() {
        cylinder(d=d-0.2, h=h, center=true);
        translate([0,0,h/2-d/2]) {
            cylinder(d1=d, d2=2*d, h=d, center=true);
        }
        translate([0,0,-h/2]) {
            metric_thread (diameter=d, pitch=0.5, length=h, internal=true, n_starts=1);
        }
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
//cup_print();
