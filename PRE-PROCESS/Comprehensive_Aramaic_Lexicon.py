import json
#CAL is  Comprehensive Aramaic Lexicon (CAL) database,
INPUT_FILE='./../RAW-DATA/dictfull.json'
OUTPUT_FILE='./../DATA/dictfull.tsv'
#giveup dialectics but
def convert_json_tsv():
    try:
        with open(file=INPUT_FILE,mode="r" ,encoding='utf-8') as file,\
            open(file=OUTPUT_FILE,mode="w" ,encoding='utf-8')   as outfile:
                
            data=(json.load(file)).items()
            
            for id,(word,entry) in enumerate(  data,start=1):
                def_segments:list[str] =[]
                entry_basic=f"{id}\t{word}"
                if(id==7):print("asd")

                for (version, v_data) in entry.items():
                    def_segments.append(f"[@{version}] ")    
                    for contents in v_data:
                        def_segments.append(f"{contents['def_num']}) {contents['definition']}")
            
                # if(id==72): break#it is for making demos not the entire json
                full_t=" ".join(def_segments)
                clean_definition= " ".join(full_t.split())  

                outfile.write(f"{id}\t{word}\t{clean_definition}\n")
            
            print("success")
            

    except FileNotFoundError:
        print('file not found')
    except json.JSONDecodeError:    
        print('error parsing json')

if __name__ == "__main__":
    convert_json_tsv()