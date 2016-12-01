from PIL import Image
import sys

if len(sys.argv) < 2:
    print "Usage: %s <image files>" % sys.argv[0]
    sys.exit(1)

names = []
anims = []
lens = []

def load_anim(filename):
    img = Image.open(filename, "r").convert("RGB")

    w, h = img.size

    if w%5 != 0 or h != 5:
        print "Image size is wrong! (%dx%d) must be (n*5,5)!" % (w,h)
        sys.exit(1)

    def is_set(x,y):
        r,g,b = img.getpixel((x,y))
        return r>0 or g>0 or b>0

    numframes = w/5
    frames = []
    for i in range(numframes):
        f = 0
        for x in range(5):
            for y in range(5):
                if is_set(x+(i*5), y):
                    f |= 1 << ((4-y) + (4-x)*5)
        frames.append(f)
    
    return frames


idx = 0
for filename in sys.argv[1:]:
    frames = load_anim(filename)
    names.append((idx,filename[filename.rfind("/")+1:].replace(".", "_").upper()))
    anims.append("{%s}" % (", ".join(map(hex, frames))))
    lens.append(str(len(frames)))
    idx += 1

print "\n".join("static const uint32_t ANIM_%s_DATA[] = %s;" % (names[i][1], anims[i]) for i in range(len(anims)))
print "\nstatic const uint32_t *ANIMS[] = {\n%s\n};" % (", ".join("ANIM_%s_DATA" % (names[i][1]) for i in range(len(names))))
print "\nstatic const uint32_t ANIM_LENS[] = {\n%s\n};\n" % (", ".join(lens))
print "\n".join("#define ANIM_%s %d" % (x,i) for (i,x) in names)
print "\n#define NUM_ANIMS %d" % (len(anims))
