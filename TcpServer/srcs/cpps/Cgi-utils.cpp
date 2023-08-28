# include "../headers/TCPsocket.hpp"

void    TCPserver::mapToCharDblPtr(void)
{
        if (myenv)
                delete[] myenv;
        myenv = new char*[preEnv.size() + 1];
        int i = 0;
        std::string fullyline;
        //std::cout << preEnv.size() << std::endl;
        for(std::map<std::string,std::string>::iterator it = preEnv.begin(); it != preEnv.end(); it++)
        {
                fullyline.clear();
                fullyline = it->first + "=";
                fullyline += it->second;
                myenv[i] = (char *)fullyline.c_str();
                //std::cout << it->first << " -> " << it->second << std::endl;
                i++;
        }
        myenv[i] = NULL;
}

void    TCPserver::createEnvForCGI(void)
{
        std::string             row;
        std::fstream    stream("custom-env.txt");
        preEnv.clear();
        while(stream)
        {
                std::getline(stream, row);
                if (row.empty())
                        break;
                preEnv[row] = "";
        }
        preEnv["PATH"] = getenv("PATH");

}
