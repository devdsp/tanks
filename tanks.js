function dbg(o) {
    e = document.getElementById("debug");
    e.innerHTML = o;
}

function torgba(color, alpha) {
    var r = parseInt(color.substring(1,3), 16);
    var g = parseInt(color.substring(3,5), 16);
    var b = parseInt(color.substring(5,7), 16);

    return "rgba(" + r + "," + g + "," + b + "," + alpha + ")";
}

function Tank(ctx, width, height, color, sensors) {
    var craterStroke = torgba(color, 0.5);
    var craterFill = torgba(color, 0.2);
    var sensorStroke = torgba(color, 0.4);
    var maxlen = 0;

    this.x = 0;
    this.y = 0;
    this.rotation = 0;
    this.turret = 0;

    // Do all the yucky math up front
    this.sensors = new Array();
    for (i in sensors) {
        var s = sensors[i];

        if (! s) {
            this.sensors[i] = [0,0,0,0];
        } else {
            // r, angle, width, turret
            this.sensors[i] = new Array();
            this.sensors[i][0] = s[0];
            this.sensors[i][1] = s[1] - s[2]/2;
            this.sensors[i][2] = s[1] + s[2]/2;
            this.sensors[i][3] = s[3]?1:0;
            if (s[0] > maxlen) {
                maxlen = s[0];
            }
        }
    }

    // Set up our state, for later interleaved draw requests
    this.set_state = function(x, y, rotation, turret, flags, sensor_state) {
        this.x = x;
        this.y = y;
        this.rotation = rotation;
        this.turret = turret;
        if (flags & 1) {
            this.fire = 5;
        }
        this.led = flags & 2;
        this.sensor_state = sensor_state;
    }

    this.draw_crater = function() {
        var points = 7;
        var angle = Math.PI / points;

        ctx.save();
        ctx.translate(this.x, this.y);
        ctx.rotate(this.rotation);

        ctx.lineWidth = 2;
        ctx.strokeStyle = craterStroke;
        ctx.fillStyle = craterFill;
        ctx.beginPath();
        ctx.moveTo(12, 0);
        for (i = 0; i < points; i += 1) {
            ctx.rotate(angle);
            ctx.lineTo(6, 0);
            ctx.rotate(angle);
            ctx.lineTo(12, 0);
        }
        ctx.closePath()
        ctx.stroke();
        ctx.fill();

        ctx.restore();
    }

    this.draw_sensors = function() {
        ctx.save();
        ctx.translate(this.x, this.y);
        ctx.rotate(this.rotation);

        ctx.lineWidth = 1;
        for (i in this.sensors) {
            var s = this.sensors[i];
            var adj = this.turret * s[3];

            if (this.sensor_state & (1 << i)) {
                // Sensor is triggered
                ctx.strokeStyle = "#000";
            } else {
                ctx.strokeStyle = sensorStroke;
            }
            ctx.beginPath();
            ctx.moveTo(0, 0);
            ctx.arc(0, 0, s[0], s[1] + adj, s[2] + adj, false);
            ctx.closePath();
            ctx.stroke();
        }

        ctx.restore();
    }

    this.draw_tank = function() {
        ctx.save();
        ctx.translate(this.x, this.y);
        ctx.rotate(this.rotation);

        ctx.fillStyle = color;
        ctx.fillRect(-5, -4, 10, 8);
        ctx.fillStyle = "#777";
        ctx.fillRect(-7, -9, 15, 5);
        ctx.fillRect(-7,  4, 15, 5);
        ctx.rotate(this.turret);
        if (this.fire) {
            ctx.fillStyle = ("rgba(255,255,64," + this.fire/5 + ")");
            ctx.fillRect(0, -1, 45, 2);
            this.fire -= 1;
        } else {
            if (this.led) {
                ctx.fillStyle = "#f00";
            } else {
                ctx.fillStyle = "#000";
            }
            ctx.fillRect(0, -1, 10, 2);
        }

        ctx.restore();
    }

    this.draw_wrap_sensors = function() {
        var orig_x = this.x;
        var orig_y = this.y;
        for (x = this.x - width; x < width + maxlen; x += width) {
            for (y = this.y - height; y < height + maxlen; y += height) {
                if ((-maxlen < x) && (x < width + maxlen) &&
                    (-maxlen < y) && (y < height + maxlen)) {
                    this.x = x;
                    this.y = y;
                    this.draw_sensors();
                }
            }
        }
        this.x = orig_x;
        this.y = orig_y;
    }
}

function start(id, game) {
    var canvas = document.getElementById(id);
    var ctx = canvas.getContext('2d');
    var loop_id;

    canvas.width = game.size.width;
    canvas.height = game.size.height;
    // game[2] is tank descriptions
    var turns = game.turns;

    // Set up tanks
    var tanks = new Array();
    for (i in game.tanks) {
        var desc = game.tanks[i];
        tanks[i] = new Tank(ctx, game.size.width, game.size.height, desc.color, desc.sensors);
    }

    var frame = 0;
    var lastframe = 0;
    var fps = document.getElementById('fps');

    function update_fps() {
        fps.innerHTML = (frame - lastframe);
        lastframe = frame;
    }

    function update() {
        var idx = frame % (turns.length + 20);
        var turn;

        frame += 1;
        if (idx >= turns.length) {
            return;
        }

        canvas.width = canvas.width;
        turn = turns[idx];

        // Draw craters first
        for (i in turn) {
            t = turn[i];
            if (! t) {
                tanks[i].draw_crater();
            }
        }
        // Then sensors
        for (i in turn) {
            t = turn[i];
            if (t) {
                // Surely there's a better way to do this.
                tanks[i].set_state(t.pos.x, t.pos.y, t.angle, t.turret, t.flags, t.sensors);
                tanks[i].draw_wrap_sensors();
            }
        }
        // Then tanks
        for (i in turn) {
            t = turn[i];
            if (t) {
                tanks[i].draw_tank()
            }
        }
    }

    loop_id = setInterval(update, 66);
    //loop_id = setInterval(update, 400);
    if (fps) {
        setInterval(update_fps, 1000);
    }
}

