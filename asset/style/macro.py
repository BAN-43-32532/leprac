def main():
    with (open("define.cpp", "r", encoding="utf-8") as define,
          open("macro.txt", "w", encoding="utf-8") as macro):
        def_start = False
        for line in define:
            if not line:
                continue
            if not def_start:
                if line.startswith("#define"):
                    def_start = True
                continue
            type, name = line.split()
            name = name.rstrip("; ")
            macro.write(f"REG_STYLE({type}, {name}),\n")


if __name__ == '__main__':
    main()
