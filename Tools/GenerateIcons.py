import os, glob, re, xml.etree.ElementTree as ET

def needs_regeneration(svg_files, output_file):
    if not os.path.exists(output_file):
        return True
    
    try:
        header_mtime = os.path.getmtime(output_file)
    except OSError:
        return True
    try:
        generator_mtime = os.path.getmtime(__file__)
    except OSError:
        return True

    if generator_mtime > header_mtime:
        return True

    for svg_path in svg_files:
        try:
            if os.path.getmtime(svg_path) > header_mtime:
                return True
        except OSError:
            continue
    
    return False

def parse_viewbox(svg_path):
    try:
        with open(svg_path, 'r', encoding='utf-8') as f:
            content = f.read()

        match = re.search(r'viewBox\s*=\s*["\']?([\d.\s\-]+)["\']?', content)
        if match:
            parts = match.group(1).replace(',', ' ').split()
            if len(parts) >= 4:
                return tuple(float(p) for p in parts[:4])

        root = ET.fromstring(content)
        vb = root.get('viewBox')
        if vb:
            parts = vb.replace(',', ' ').split()
            if len(parts) >= 4:
                return tuple(float(p) for p in parts[:4])
    except:
        pass
    return None

def generate_icons():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    icons_dir  = os.path.abspath(os.path.join(script_dir, "..", "Icons"))
    output_dir = os.path.abspath(os.path.join(script_dir, "..", "Generated"))
    os.makedirs(output_dir, exist_ok=True)
    
    output_file = os.path.join(output_dir, "IconsEmbedded.h")
    svg_files = sorted(glob.glob(os.path.join(icons_dir, "*.svg")))
    
    if not needs_regeneration(svg_files, output_file):
        print('[v] Icons already up to date')
        return

    if not svg_files:
        print(f"[!] Icons not found: {icons_dir}")
        return False

    with open(output_file, "w", encoding="utf-8") as f:
        f.write("#pragma once\n#include <cstddef>\n\n")
        f.write("struct EmbeddedIcon {\n")
        f.write("    const char* data;\n")
        f.write("    std::size_t size;\n")
        f.write("    float viewBoxX, viewBoxY, viewBoxW, viewBoxH;\n")
        f.write("    bool hasViewBox;\n")
        f.write("};\n\nnamespace IconsData {\n\n")
        
        for svg_path in svg_files:
            name = os.path.splitext(os.path.basename(svg_path))[0]
            with open(svg_path, "rb") as sf:
                data = sf.read()
            
            vb = parse_viewbox(svg_path)
            if vb:
                vb_x, vb_y, vb_w, vb_h = vb
                has_vb = "true"
            else:
                vb_x, vb_y, vb_w, vb_h = 0, 0, 100, 100
                has_vb = "false"
            
            f.write(f"inline const char {name}_svg_data[] = {{\n")
            for i, b in enumerate(data):
                f.write(f"0x{b:02X}, ")
                if (i + 1) % 16 == 0: f.write("\n")
            f.write("\n};\n")
            
            f.write(f"inline constexpr EmbeddedIcon {name} {{\n")
            f.write(f"    {name}_svg_data,\n")
            f.write(f"    {len(data)},\n")
            f.write(f"    {vb_x}f, {vb_y}f, {vb_w}f, {vb_h}f,\n")
            f.write(f"    {has_vb}\n")
            f.write("};\n\n")
        
        f.write("} // namespace IconsData\n")
    
    print(f"[+] Generated {output_file} ({len(svg_files)} icons with viewBox)")
    return True

if __name__ == "__main__":
    generate_icons()