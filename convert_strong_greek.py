import xml.etree.ElementTree as ET

xml_file="strongsgreek.xml"
tsv_file="strongsgreek_c.tsv" 

tree = ET.parse(xml_file)
root = tree.getroot()

try:
    context = ET.iterparse(xml_file, events=("end",))

    with open( tsv_file, "w" ,encoding="utf-8" ) as f:
        f.write("id\tunicode_word\ttranslit\tpronunciation\tfull_definition\n")
    
        for event, elem in context:
            if elem.tag == "entry":
                strong_id =elem.get("strongs").strip()
            
                greek_node = elem.find("greek")
                unicode_word= greek_node.get("unicode","") if greek_node is not None else ""
                translit =greek_node.get("translit","")if greek_node is not None else ""

                pron_node=elem.find("pronunciation")
                pron=pron_node.get("strongs","")if pron_node is not None else ""

                raw_text_segments= list(elem.itertext())
                #clean whitespaces/tabs
                full_t=" ".join(raw_text_segments)
                clean_definition= " ".join(full_t.split())

                f.write(f"G{strong_id}\t{unicode_word}\t{translit}\t{pron}\t{clean_definition}\n")
                elem.clear()

    print("succes: XML flattened")
except FileNotFoundError:
    print(f"Error: could not find '{xml_file}' file")    
except ET.ParseError as e:
    print(f"Error:parsing xml structure {e}")       


