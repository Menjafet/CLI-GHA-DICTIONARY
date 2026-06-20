import xml.etree.ElementTree as ET

xml_file="stronghebrewg.xml"
tsv_file="strongHebrewG_c.tsv" 

#tree = ET.parse(xml_file)
#root = tree.getroot()

try:
    context = ET.iterparse(xml_file, events=("end",))

    with open( tsv_file, "w" ,encoding="utf-8" ) as f:
        f.write("id\tlemma\tpure\ttranslit\tfull_definition\n")
        dir="{http://www.bibletechnologies.net/2003/OSIS/namespace}"
        
        for event, elem in context:
        #1            
            if elem.tag == dir+"div":
                
                #and elem.get("type").strip()=="entry"
#"H8552":{"lemma":"תָּמַם","xlit":"tâmam","pron":"taw-mam'","derivation":"a primitive root;","strongs_def":"to complete, in a good or a bad sense, literal, or figurative, transitive or intransitive","kjv_def":"accomplish, cease, be clean (pass-) ed, consume, have done, (come to an, have an, make an) end, fail, come to the full, be all gone, [idiom] be all here, be (make) perfect, be spent, sum, be (shew self) upright, be wasted, whole."},
                hebrew_node = elem.find(dir+"w")
                
                if hebrew_node is not None:
                    lemma_word= hebrew_node.get("lemma","").strip()
                    strong_id =hebrew_node.get("ID").strip()
                    stripped_word=hebrew_node.text if hebrew_node.text else ""
                    translit =hebrew_node.get("xlit","").strip()
                else:
                    elem.clear()
                    continue
                def_segments =[]

                for child in elem:
                    #1
                    if child.tag == dir+"w":
                        continue
                    #2
                    if child.tag == dir+"foreign":
                        greek_refs=[]
                        for foreign_w in child.findall(dir+"w") :
                            gloss =foreign_w.get("gloss","")
                            if gloss:
                                clean_gloss = gloss.replace("G:","G").strip()
                                greek_refs.append(clean_gloss)
                        if greek_refs:
                            def_segments.append(f"[G:references: {','.join(greek_refs)}]")
                        continue    

                        def_segments.append(child_text)
                    #3
                    child_text ="".join(child.itertext()).strip()
                    if child_text:
                        def_segments.append(child_text)


                #clean whitespaces/tabs
                full_t=" ".join(def_segments)
                clean_definition= " ".join(full_t.split())  

                f.write(f"{strong_id}\t{lemma_word}\t{stripped_word}\t{translit}\t{clean_definition}\n")
                elem.clear()

    print("succes: XML flattened")
except FileNotFoundError:
    print(f"Error: could not find '{xml_file}' file")    
except ET.ParseError as e:
    print(f"Error:parsing xml structure {e}")       


