#include "config.hpp"
int readConf(std::string file , std::string conf , std::string& readTo){
	std::ifstream ifile(file);
	std::string str;
	if(ifile.is_open()){
		for ( int i = 0 ; std::getline(ifile,str) ; i++) {
			if(!str.size())continue;
			
			int semi = str.find(":");
			int hash = str.find("#");
			int textStart = 0;
			int textEnd =  0;
			int startOfline = 0;
			
			for(; (str[startOfline] == ' ' || str[startOfline] =='\t') && startOfline<str.size();startOfline++);;
				
			if(startOfline == str.size())continue;
			if(semi == -1)continue;
			if(hash != -1 && hash < semi )continue;
			
			std::string co = str.substr(startOfline,semi);	
			if(co==conf){
				for(int ii = semi, chek = 0 ; ii<str.size(); ii++){
					if(str[ii] == '\"'){
						if(chek == 0){
							
								textStart = ii+1;
								chek++;
							
						}else if(chek == 1){
							textEnd = ii ;
							break;
						}
					}
				}
				if(textStart > semi && textStart && textEnd && textEnd > textStart){
					readTo = str.substr(textStart , textEnd - textStart );
					return 1;
				}
			}
		
		}

		ifile.close();
	}else {
		std::cout << "\n[ERROR]: never finde config file \""<<file <<".";
		return 0;
	}
	return 0;
}



bool therIsAfile(std::string file){
	std::ifstream ifi(file);
	bool is_open = ifi.is_open();
	ifi.close();
	return is_open;
}


int writeConf(std::string file , std::string conf , std::string valu){
	std::ifstream ifi(file);
	std::string str;
	std::string fullCf;
	if(ifi.is_open()){
		bool isFond = false;
		for ( int i = 0 ; std::getline(ifi,str) ; i++) {
			if(!str.size())continue;
			
			int semi = str.find(":");
			int hash = str.find("#");
			int textStart = 0;
			int textEnd =  0;
			int startOfline = 0;
			
			for(; (str[startOfline] == ' ' || str[startOfline] =='\t') && startOfline<str.size();startOfline++);;
			
			if(startOfline == str.size())continue;
			if(semi == -1)continue;
			if(hash != -1 && hash < semi )continue;
			
			std::string co = str.substr(startOfline,semi);	
			if(co==conf){
				fullCf.append(conf);
				fullCf.append(":");
				fullCf.append("\"");
				fullCf.append(valu);
				fullCf.append("\"\n");
				isFond = true;
				
			}else {
				fullCf.append(str);
				fullCf.append("\n");
			}
		
		}
		std::ofstream ofi(file);
		if(isFond){
			ofi<<fullCf;
		}else {
			fullCf.append(conf);
			fullCf.append(":");
			fullCf.append("\"");
			fullCf.append(valu);
			fullCf.append("\"\n");
			ofi<<fullCf;
		}
		//std::cout << fullCf;
		ifi.close();
		return 1;
	}
	else{
		std::ofstream ofi(file);
		fullCf.append(conf);
		fullCf.append(":");
		fullCf.append("\"");
		fullCf.append(valu);
		fullCf.append("\"\n");
		ofi<<fullCf;
	}
	return 0;	
	


}


bool IsTherConfig(std::string file, std::string conf){
	std::ifstream ifile(file);
	std::string str;
	if(ifile.is_open()){
		for ( int i = 0 ; std::getline(ifile,str) ; i++) {
			if(!str.size())continue;
			
			int semi = str.find(":");
			int hash = str.find("#");
			int textStart = 0;
			int textEnd =  0;
			int startOfline = 0;
			
			for(; (str[startOfline] == ' ' || str[startOfline] =='\t') && startOfline<str.size();startOfline++);;
				
			if(startOfline == str.size())continue;
			if(semi == -1)continue;
			if(hash != -1 && hash < semi )continue;
			
			std::string co = str.substr(startOfline,semi);	
			if(co==conf){return true;}
		}

		ifile.close();
		return false;
	}else {
		std::cout << "\n[ERROR]: never finde config file \""<<file <<".";
		return false;
	}
	return false;
}

