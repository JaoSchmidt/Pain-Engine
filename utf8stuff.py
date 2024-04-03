import glob, codecs

for f in glob.glob("./**/*.cpp",recursive=True):
    print(f)
    data = open(f, "rb").read()
    if data.startswith(codecs.BOM_UTF8):
        # Already UTF-8
        continue
    # else assume ANSI code page
    data = data.decode("mbcs")
    data = codecs.BOM_UTF8 + data.encode("utf-8")
    
    open(f, "wb").write(data)