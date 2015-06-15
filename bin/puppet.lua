--
-- puppet.lua -- Introduction to Computer Graphics
-- 

rootnode = gr.node('root')

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
purple = gr.material({1.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
yellow = gr.material({1.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
orange = gr.material({0.3, 0.7, 0.5}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
black = gr.material({0.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)

--torso joint
toj = gr.joint('toj', {-90.0, 0.0, 90.0}, {-90.0, 0.0, 90.0})
rootnode:add_child(toj)


--torso
to = gr.sphere('to')
toj:add_child(to) 
to:set_material(yellow)
to:scale(1,2,0.5)
to:translate(0,-0.5,0)


--hips joint
hipj = gr.joint('hipj', {-90.0, 0.0, 90.0}, {-90.0, 0.0, 90.0})
toj:add_child(hipj)
hipj:translate(0,-3,0)

--hips
hip = gr.sphere('hip')
hipj:add_child(hip) 
hip:set_material(yellow)
hip:scale(1,0.5,0.5)
--hip:translate(0,-5.5,0)


--left thigh
ltj = gr.joint('ltj', {-90.0, 0.0, 90.0}, {-90.0, 0.0, 90.0})
hipj:add_child(ltj)
ltj:translate(-0.5,-1,0)

lt = gr.sphere('lt')
ltj:add_child(lt)
lt:set_material(yellow)
lt:scale(0.4,1,1)


--right thigh
rtj = gr.joint('rtj', {-90.0, 0.0, 90.0}, {-90.0, 0.0, 90.0})
hipj:add_child(rtj)
rtj:translate(0.5,-1,0)

rt = gr.sphere('rt')
rtj:add_child(rt)
rt:set_material(yellow)
rt:scale(0.4,1,1)



--left calf
lcj = gr.joint('lcj', {-90.0, 0.0, 90.0}, {-90.0, 0.0, 90.0})
ltj:add_child(lcj)
lcj:translate(0,-1,0)

lc = gr.sphere('lc')
lcj:add_child(lc)
lc:set_material(yellow)
lc:scale(0.4,1,1)


--right calf
rcj = gr.joint('rcj', {-90.0, 0.0, 90.0}, {-90.0, 0.0, 90.0})
rtj:add_child(rcj)
rcj:translate(0,-1,0)

rc = gr.sphere('rc')
rcj:add_child(rc)
rc:set_material(yellow)
rc:scale(0.4,1,1)


--left foot
lfj = gr.joint('lfj', {-90.0, 0.0, 90.0}, {-90.0, 0.0, 90.0})
lcj:add_child(lfj)
lfj:translate(0,-1,0)

lf = gr.sphere('lf')
lfj:add_child(lf)
lf:set_material(yellow)
lf:scale(0.4,0.5,1.5)


--right foot
rfj = gr.joint('rfj', {-90.0, 0.0, 90.0}, {-90.0, 0.0, 90.0})
rcj:add_child(rfj)
rfj:translate(0,-1,0)

rf = gr.sphere('rf')
rfj:add_child(rf)
rf:set_material(yellow)
rf:scale(0.4,0.5,1.5)




--Shoulder joint
sh = gr.joint('sh', {-90.0, 0.0, 90.0}, {-90.0, 0.0, 90.0})
toj:add_child(sh)


--Shoulder
sh1 = gr.sphere('sh1')
sh:add_child(sh1) 
sh1:set_material(yellow)
sh1:scale(3,0.5,0.5)
sh1:translate(0,1,0)

--neck
nj = gr.joint('nj', {-90.0, 0.0, 90.0}, {-90.0, 0.0, 90.0})
nj:translate(0,1,0)
sh:add_child(nj)

n = gr.sphere('n')
nj:add_child(n) 
n:set_material(yellow)
n:scale(0.5,0.7,0.5)



--head
hj = gr.joint('hj', {-90.0, 0.0, 90.0}, {-90.0, 0.0, 90.0})
hj:translate(0,1.5,0)
nj:add_child(hj)

h = gr.sphere('h')
hj:add_child(h) 
h:set_material(yellow)
h:scale(1,1,1)


--nose
no = gr.sphere('no')
hj:add_child(no)
no:set_material(yellow)
no:scale(0.4,0.4,1)
no:translate(0,0,0.5)

--Left Arm
j0 = gr.joint('j0', {-90.0, 0.0, 90.0}, {-90.0, 0.0, 90.0})
sh:add_child(j0)

s0 = gr.sphere('s0')
j0:add_child(s0)
s0:set_material(yellow)
s0:scale(0.5,1,0.5)

j1 = gr.joint('j1', {-90.0, 0.0, 90.0}, {-90.0, 0.0, 90.0})
j0:add_child(j1)

s1 = gr.sphere('s1')
j1:add_child(s1)
s1:set_material(blue)
s1:scale(0.5,1,0.5)
s1:translate(0,-1.5,0)

j2 = gr.joint('j2', {-90.0, 0.0, 90.0}, {-90.0, 0.0, 90.0})
j1:add_child(j2)

s2 = gr.sphere('s2')
j2:add_child(s2)
s2:set_material(blue)
s2:scale(0.5,0.5,0.5)
s2:translate(0,-4.5,0)

--Right Arm
j3 = gr.joint('j3', {-90.0, 0.0, 90.0}, {-90.0, 0.0, 90.0})
sh:add_child(j3)

s3 = gr.sphere('s3')
j3:add_child(s3)
s3:set_material(yellow)
s3:scale(0.5,1,0.5)

j4 = gr.joint('j4', {-90.0, 0.0, 90.0}, {-90.0, 0.0, 90.0})
j3:add_child(j4)

s4 = gr.sphere('s4')
j4:add_child(s4)
s4:set_material(blue)
s4:scale(0.5,1,0.5)
s4:translate(0,-1.5,0)

j5 = gr.joint('j5', {-90.0, 0.0, 90.0}, {-90.0, 0.0, 90.0})
j4:add_child(j5)

s5 = gr.sphere('s5')
j5:add_child(s5)
s5:set_material(blue)
s5:scale(0.5,0.5,0.5)
s5:translate(0,-4.5,0)


rootnode:translate(0, 0, -20)
j0:translate(-2,0,0)
j3:translate(2,0,0)

return rootnode