/**
*
* @file      script.C
* @brief     define script storage and interfaces
* @author    chenggang
* @History
*    1.0.0: chenggang,  2017/06/19,  Initial version
*/

#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>  
#include <libxml/xpathInternals.h>  
#include <libxml/xmlmemory.h>  
#include <libxml/xpointer.h> 
#include "script.h"

TScript::TScript(const char * file)
{
   m_file = file;
   m_script = NULL;
}

TScript::~TScript()
{
   exit();
}

bool TScript::init()
{
   xmlDocPtr   pDoc = NULL;
   xmlNodePtr  pRoot = NULL;
   xmlNodePtr  pNode = NULL;
   
   TStep*      pStep = NULL;

   pDoc = xmlReadFile (m_file, "UTF-8", XML_PARSE_RECOVER);
   if (NULL == pDoc)
   {
      printf("ERROR: TScript can't open file[%s]!\n", m_file.c_str());
      return false;
   }

   pRoot = xmlDocGetRootElement(pDoc);
   if (NULL == pRoot)
   {
      printf("ERROR: TScript check XML file[%s] is invalid!\n", m_file.c_str());
      return false;
   }
   if (xmlStrcmp(pRoot->name, (const xmlChar*) "root"))
   {
      printf("TScript check XML type wrong, root node != root");
      xmlFreeDoc(pDoc);
      return false;
   }

   pNode = pRoot->xmlChildrenNode;
   while(pNode)
   {
      pStep = initStep(pNode, pStep);
      if(NULL = pStep)
      {
         break;
      }

      if(NULL == m_script)
      {
         m_script = pStep;
      }
      if(pLast)
      {
         pLast->next = pStep;
      }
      pNode = pNode->next;
      pLast = pStep;
   }

   if(pNode)   //step init failed, delete all step.
   {
      pStep = m_script;
      while(pStep)
      {
         pLast = pStep->next;
         delete pStep;
         pStep = pLast;
      }
      xmlFreeDoc(pDoc);
      return false;
   }
   
   if(NULL == m_script)
   {
      printf("TScript init failed: can't get valid script");
      xmlFreeDoc(pDoc);
      return false;
   }

   xmlFreeDoc(pDoc);
   return true;
   
}

TStep* TScript::initProperty(xmlNodePtr pNode)
{
   xmlChar* pValue = NULL;
   TStep*   pStep = NULL;
   bool     ret = false;
   
   pValue = xmlGetProp(pNode, (const xmlChar *)"protocol");
   if(strncasecmp("HTTP", pValue) == 0)
   {
      pStep = new THTTPStep();
      ret = (THTTPStep *)pStep->init(pNode);
   }
   else if(strncasecmp("TCP", pValue) == 0)
   {
      pStep = new TTCPStep();
      ret = (TTCPStep *)pStep->init(pNode);
   }
   else if(strncasecmp("UDP", pValue) == 0)
   {
      pStep = new TUDPStep();
      ret = (TUDPStep *)pStep->init(pNode);
   }
   else if(strncasecmp("FTP", pValue) == 0)
   {
      pStep = new TFTPStep();
      ret = (TFTPStep *)pStep->init(pNode);
   }
   else if(strncasecmp("RMDB", pValue) == 0)
   {
      pStep = new TRMDBStep();
      ret = (TRMDBStep *)pStep->init(pNode);
   }
   else
   {
      printf("TScript initProperty() find unknow protocol type(%s)", pValue);
      return NULL;
   }
   
   if(false == ret)
   {
      delete pStep;
      pStep = NULL;
      return NULL;
   }
}

TStep* TScript::enter()
{
}

bool TScript::exit()
{
   TStep*      pStep = NULL;
   
   while(NULL == m_scripst)
   {
      pStep = m_scripst->next;
      delete m_scripst;
      m_scripst = pStep;
   }
   m_scripst = NULL;
}


