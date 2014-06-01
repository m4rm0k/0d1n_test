#include "spider.h"
#define TABLE "tables/output_array.txt"
#define TEMPLATE "template.conf"
#define TEMPLATE2 "hammer1.conf"
#define TEMPLATE3 "hammer2.conf"

size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data) 
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)data;

	mem->memory = realloc(mem->memory, mem->size + realsize + 1);

	if(mem->memory) 
	{
		memcpy(&(mem->memory[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;
	}

	return realsize;
}

void spider(void *pack,char *line,char * pathtable)
{
	FILE *fp=NULL;
	bool match_string=false;
	long status=0;
	int old=0,counter=0,POST=0,first=0,last=0; 
	char *make=NULL,*pathsource=NULL;
        char *data=NULL,*method=NULL,*request_file=NULL,*line_p=NULL,*ptr=NULL; 
	char **pack_ptr=(char **)pack,**arg = pack_ptr;
	char tabledata[6660],randname[16],log[5025],line2[1024];

	CURL *curl;  
	curl_global_init(CURL_GLOBAL_ALL); 
	struct curl_slist *headers2 = NULL;
	struct MemoryStruct chunk;

	POST=(arg[4]==NULL)?0:1;
   
	counter=char_type_counter(POST?arg[4]:arg[0],'!');
	old=counter;  
	chomp(line);

// xxx...custom..request...xxx special char counter
	if(arg[11]!=NULL)
	{
                request_file=(char*)malloc(3000*sizeof(char));
                request_file=readLine(arg[11]);
                old=char_type_counter(request_file,'!');

		if(old>=1)
			puts("\nOK\n");
		else {
			puts("\n problem at custom request file need special char to make attack...\n");
		} 
	}
// xxxxxxxxxxxxxxxxxxxxxxxxxxx

	while(old)
	{
		chunk.memory=NULL; 
		chunk.size = 0; 
 
		curl = curl_easy_init();

		if(arg[11]==NULL) 
		{
			make=payload_injector( (POST?arg[4]:arg[0]),line,old);
	 		curl_easy_setopt(curl,  CURLOPT_URL, POST?arg[0]:make);
		} else {
// to use custom request
			make=payload_injector(request_file,line,old);
			data=(char *)malloc(1024*sizeof(char)),method=(char *)malloc(1024*sizeof(char)); 
			memset(data,0,strlen(data));
			memset(method,0,strlen(method));
//			split_request(headers,data,method,arg[11]);
	 		curl_easy_setopt(curl,  CURLOPT_URL, arg[0]);
puts("debug 0");
		}    

		if(POST)
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, make);
// custom request
		if(arg[11]!=NULL)
		{
			ptr=(char *)malloc(strlen(make)*sizeof(char)+1);
//change this 2 strlcpy()
			strcpy(ptr,make);
                        line_p=(char *)malloc(sizeof(char)*1024);
                        short pos=0;
puts("memset1"); 
			memset(data,0,256);
			memset(method,0,256);
                        
puts("debug strcpy");                              	
		
			while(*ptr!='\0') 
			{
				if(!first)
				{
					*(method+pos)=*ptr;
                                 	if(*ptr=='\n')
					{
						first=1;
						pos=0;
                                 	}
				} else {
					*(line_p+pos)=*ptr;
					if(*ptr=='\n' && strlen(line_p)>=5 && !last)
					{
// here be dragons, bugs bugs everywhere HAHAHA, zi i need ob other form to fix this 
						line_p=StrRep(line_p,"\n","",512);
						line_p=StrRep(line_p,"\r","",512);
						line_p=StrRep(line_p,"\t","",512);
						line_p=StrRep(line_p,"\r\n","",512);

						printf("header: %s\n",line_p);
						headers2=curl_slist_append(headers2,line_p);
						memset(line_p,0,strlen(line_p)-1);
//						free(&line_p);
  //                      	 		line_p=(char *)malloc(sizeof(char)*1024);
						pos=0;	
					} 

					if(*ptr=='\n' && strlen(line_p)==0) 
					{
						last=1;
						pos=0;
					}	
				}

                                if(last)
				{
				 *(data+pos)=*ptr;
				}
				ptr++;
				pos++;
			}

			method=StrRep(method,"\n","",512);
			method=StrRep(method,"\r","",512);
			method=StrRep(method,"\t","",512);
			method=StrRep(method,"\r\n","",512);
                        last=0;
                        first=0;
puts("debug free parse");
                        if(ptr==NULL)
                        	free(ptr);
	//		memset(line_p,0,strlen(line_p));
                        if(line_p==NULL)
				free(line_p);

printf("DEBUG_PARSE::: DATA : %s\n method: %s\n",data,method);

 
		}
puts("debug1"); 
		curl_easy_setopt(curl,  CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl,  CURLOPT_WRITEDATA, (void *)&chunk);
    
		if(arg[6]!=NULL)
		{
			curl_easy_setopt(curl,  CURLOPT_USERAGENT, arg[6]);
		} else if(arg[11]==NULL){
			curl_easy_setopt(curl,  CURLOPT_USERAGENT, "Mozilla/5.0 (0d1n v0.1) ");
		}

		if(arg[11]==NULL)
			curl_easy_setopt(curl,  CURLOPT_ENCODING,"gzip,deflate");

		if(arg[3]!=NULL)
		{
			curl_easy_setopt(curl,CURLOPT_COOKIEFILE,arg[3]);
			curl_easy_setopt(curl,CURLOPT_COOKIEJAR,arg[3]);
		} else if(arg[11]==NULL){
			curl_easy_setopt(curl,CURLOPT_COOKIEJAR,"odin_cookiejar.txt");
		}

		curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1);

		if(arg[7]!=NULL) 
		{
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2);
			curl_easy_setopt(curl, CURLOPT_CAINFO, arg[7]);
		} else {
			curl_easy_setopt(curl,CURLOPT_SSL_VERIFYPEER,0); 
		}

		if(arg[8]!=NULL) 
			curl_easy_setopt(curl,CURLOPT_TIMEOUT,atoi(arg[8])); 

		if(arg[9]!=NULL) 
			curl_easy_setopt(curl,CURLOPT_SSLVERSION,atoi(arg[9]));
 // custom request
		if(arg[11]!=NULL)
		{
                  
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers2); 
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
                      printf("debug data send: %s\n",data);
puts("debug2");    
		} else {
			curl_easy_setopt(curl,CURLOPT_HEADER,1); 
		}
 
		curl_easy_setopt(curl,CURLOPT_VERBOSE,1); 
		curl_easy_perform(curl);
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE,&status);
		curl_easy_cleanup(curl);
puts("debug3");
// custom request free parse data
		if(arg[11]!=NULL)
		{ /*
			if(method!=NULL)
				free(method); */
puts("debug003");
			if(data!=NULL)
				free(data);	
puts("debug33");
			curl_slist_free_all(headers2);

		}

 printf("\n status %lu\n",status);
// printf("%s\n",chunk.memory);
puts("debug4");
                 
// arg[10]  list to find with regex , arg[2] list without regex
		if( (arg[2] || arg[10] ) && status != 0 )
		{
			fp = fopen((arg[2]!=NULL)?arg[2]:arg[10], "r");
			if(!fp)
			{ 
				puts("error to open response list"); 
				exit(0);
			}


			while(fgets(line2,1023,fp) != NULL) 
			{
				chomp(line2);


				if(arg[2]!=NULL)
					match_string=strstr(chunk.memory,line2)?true:false;

				if(arg[10]!=NULL)
					match_string=strstr_regex(chunk.memory,line2)?true:false;

				if(chunk.memory && (match_string == true) ) 
				{
					fprintf(stdout,"%s [ %s %lu %s ] Payload: %s %s %s Grep: %s %s %s  Params: %s %s\n",YELLOW,CYAN,status,YELLOW,GREEN,line,YELLOW,CYAN,line2,YELLOW,make,LAST);
					pathsource=(char *)malloc(sizeof(char)*64);
					memset(pathsource, 0,sizeof(char)*64);
					strlcat(pathsource,"response_sources/",63);
					strlcat(pathsource,arg[5],63);
					mkdir(pathsource,S_IRWXU|S_IRWXG|S_IRWXO);
					strlcat(pathsource,"/",63);
					strlcat(pathsource,rand_str(randname, sizeof randname),63);
					strlcat(pathsource,".html",63);
					snprintf(log,5023,"[%lu] Payload: %s  Grep: %s Params: %s \n Path Response Source: %s\n",status,line,line2,make,pathsource);
					WriteFile(arg[5],log);
					WriteFile(pathsource,readLine(TEMPLATE));
					WriteFile(pathsource,html_entities(chunk.memory));
					WriteFile(pathsource,"</pre></html>");

					snprintf(tabledata,6659,"[\"<a href=\\\"../%s\\\">%lu </a>\",\"%s\",\"%s\",\"%s\"],\n",pathsource,status,html_entities(make),
					html_entities(line2),html_entities(line));
					WriteFile(pathtable,tabledata);
					free(pathsource);
				}
			}

 			fclose(fp);

		} else {
			fprintf(stdout,"%s [ %s %lu %s ] Payload: %s %s %s Params: %s %s %s\n",YELLOW,CYAN,status,YELLOW,GREEN,line,YELLOW,CYAN,make,LAST);
			pathsource=(char *)malloc(sizeof(char)*64);
			memset(pathsource, 0,sizeof(char)*64);
			strlcat(pathsource,"response_sources/",63);
 			strlcat(pathsource,arg[5],63);
			mkdir(pathsource,S_IRWXU|S_IRWXG|S_IRWXO);
			strlcat(pathsource,"/",63);
			strlcat(pathsource,rand_str(randname, sizeof randname),63);
 			strlcat(pathsource,".html",63);
			snprintf(log,5023,"[%lu] Payload: %s Params: %s \n Path Response Source: %s\n",status,line,make,pathsource);

			WriteFile(arg[5],log);
			WriteFile(pathsource,readLine(TEMPLATE));
			WriteFile(pathsource,html_entities(chunk.memory));
			WriteFile(pathsource,"</pre></html>");
			snprintf(tabledata,6659,"[\"<a href=\\\"../%s\\\">%lu </a>\",\"%s\",\"%s\",\"%s\"],\n",pathsource,status,html_entities(make),
			html_entities(line2),html_entities(line));
			WriteFile(pathtable,tabledata);
			free(pathsource);
		}
puts("debug5");
		if(make)
			free(make);

		if(chunk.size <10) 
			free(chunk.memory);

		old--;
	}

	if(request_file)
		free(request_file);


}


void scan(void *arguments)
{
	FILE *fp=NULL;

	char **arg = (char **)arguments;
	char *pathtable=NULL,*pathhammer=NULL,*view;
	char line[2048]; 

 
	pathtable=(char *)malloc(sizeof(char)*64);
	memset(pathtable, 0, sizeof(char)*64);
	strlcat(pathtable,"tables/",63);
	strlcat(pathtable,arg[5],63);
	strlcat(pathtable,".txt",63);
	fp = fopen(arg[1], "r");

	if(!fp)
	{ 
 		puts("error to open Payload list"); 
		exit(1);
	}
 
	view=(char *)malloc(sizeof(char)*2048);
	memset(view, 0,sizeof(char)*2048);

	strlcat(view,readLine(TEMPLATE2),2047);
	strlcat(view,"\"sAjaxSource\": \"",2047);
	strlcat(view,arg[5],2047);
 	strlcat(view,".txt\" \n",2047);
	strlcat(view,readLine(TEMPLATE3),2047);

	pathhammer=(char *)malloc(sizeof(char)*64);
	memset(pathhammer, 0,sizeof(char)*64);
 	strlcat(pathhammer,"tables/",63);
 	strlcat(pathhammer,arg[5],63);
	strlcat(pathhammer,"_hammer.html",63);
	WriteFile(pathhammer,view);
	WriteFile(pathtable,"{ \"aaData\": [ \n");


	while(fgets(line,2047,fp) != NULL) 
		spider(arguments,line,pathtable);
  
	WriteFile(pathtable," [\"\",\"\",\"\",\"\"] \n ] }");

	puts(RED);
	fprintf(stdout,"end scan \n look the file %s\n long life to Wotan\n",pathhammer);
	puts(LAST);


	if(pathtable)
 		free(pathtable);

	if(pathhammer)
		free(pathhammer);

	if(view)
		free(view);

	fclose(fp);

	exit(0);

}
