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
                
    #<div type="entry" n="3508">
    #   <w gloss="936e" lemma="יֹתֶרֶת" morph="n-f" POS="yo-theh'-reth" xlit="yôthereth" ID="H3508" xml:lang="heb">יתרת</w>
    #   <list>
    #     <item>1) appendage,  overhang,  protrusion,  the caudate lobe of the liver of a sacrificial animal</item>
    #   </list>
    #   <note type="exegesis">feminine active participle of <w lemma="יָתַר" POS="yaw-thar'" src="3498" xlit="yâthar"/>;</note>
    #   <note type="explanation"><hi>the lobe</hi> or <hi>flap of the liver</hi> (as if redundant or outhanging)</note>
    #   <note type="translation">caul.</note>
    # </div>
                hebrew_node = elem.find(dir+"w")
                def_segments =[]

                for child in elem:
                    #1
                    if child.tag == dir+"w":
                        lemma_word  =child.get("lemma","").strip()
                        strong_id=  child.get("ID") if  child.get("ID") is not None else ""
                        if strong_id=="H3498":
                            print("SSSS")
                        stripped_word=  child.text if  child.text else ""
                        translit =  child.get("xlit","").strip()
                        continue
                    #2
                    if child.tag == dir+"list":
                        
                        for item in child:
                            def_segments.append(item.text)
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
                    #3
                    if child.tag==dir+"note":
                        type=child.get("type","").strip()
                        deriv=[child.text] if child.text is not None else [""]
                        for part in child:
                            if part.tag == dir+"w":
                                reflemma=part.get("lemma","")if child is not None else ""
                                resrc=part.get("src","")if child is not None else ""
                                deriv.append(f"{reflemma} H{resrc} ")
                            if part.tag== dir+"hi":
                                hi_cont=part.text if part.text is not None else ""
                                deriv.append(hi_cont)
                            if part.tail:
                                deriv.append(part.tail)
                        
                        deriv_result:list=deriv if len(deriv)>0  else [""]
                        deriv_str=" ".join(deriv_result)
                        c_note=f"{type}:{deriv_str}"
                        def_segments.append(c_note)
                    child_text ="".join(child.itertext()).strip()
                    #if child_text:
                    #    def_segments.append(child_text)


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


