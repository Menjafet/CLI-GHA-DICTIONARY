import xml.etree.ElementTree as ET

xml_file="strongsgreek.xml"
tsv_file="strongsgreek_c.tsv" 

tree = ET.parse(xml_file)
root = tree.getroot()
def sigla(lang:str):
    if(lang=="GREEK"):
        return "G"
    if(lang=="HEBREW"):
        return "H"
    return ""
try:
    context = ET.iterparse(xml_file, events=("end",))
    with open( tsv_file, "w" ,encoding="utf-8" ) as f:
        f.write("id\tunicode_word\ttranslit\tpronunciation\tfull_definition\n")
    
        for event, elem in context:
            if elem.tag == "entry":
                strong_id ="G"
                def_segments=[]

                for child in elem:
                    if child.tag=="strongs":
                        strong_id=strong_id+child.text.strip()
                        if(strong_id=="G800"):
                            print('sd')
                    if child.tag=="greek":
                        unicode_word= child.get("unicode","") if child is not None else ""
                        translit =child.get("translit","")if child is not None else ""
                    if child.tag=="strongs_derivation":
                        deriv=[child.text]
                        for part in child:
                            if part.tag == "strongsref":
                                reftxt=part.get("strongs","")if child is not None else ""
                                reflan=part.get("language","")if child is not None else ""
                                deriv.append(f"{sigla(reflan)}{reftxt} ")
                            if part.tail:
                                deriv.append(part.tail)
                        
                        def_segments.append(" ".join(deriv))
                    if child.tag=="strongs_def":
                        def_segments.append("def:"+child.text)
                    if child.tag=="kjv_def":
                        def_segments.append("kjv"+child.text)
                    if child.tag=="see":
                       seetxt=child.get("strongs","")if child is not None else ""
                       seelan=child.get("language","")if child is not None else ""
                       def_segments.append(f"see {sigla(seelan)}{seetxt},")
                       
                
            
                pron_node=elem.find("pronunciation")
                pron=pron_node.get("strongs","")if pron_node is not None else ""
   
                #clean whitespaces/tabs
                full_t=" ".join(def_segments)
                clean_definition= " ".join(full_t.split())

                f.write(f"{strong_id}\t{unicode_word}\t{translit}\t{pron}\t{clean_definition}\n")
                elem.clear()

    print("succes: XML flattened")
except FileNotFoundError:
    print(f"Error: could not find '{xml_file}' file")    
except ET.ParseError as e:
    print(f"Error:parsing xml structure {e}")       


