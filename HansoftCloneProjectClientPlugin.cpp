
#include "../../Projects/HansoftSDK_7_105/HPMSdkCpp.h"

#include <tchar.h>
#include <conio.h>
#include <windows.h>

#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include <memory>

#define mod_export __declspec(dllexport)

using namespace std;
using namespace HPMSdk;

class CHansoftCloneProject_ClientPlugin : public HPMSdkCallbacks
{
public:

	struct CDynamicHelper
	{
		HPMNotificationSubscription m_RightClickSubscription;
		HPMNotificationSubscription m_DynamicUpdateSubscription;
	};

	shared_ptr<CDynamicHelper> m_pDynamicHelper;
	HPMString m_CustomDialogSpec;
	HPMSdkSession *m_pSession;
	HPMString m_IntegrationIdentifier;
	

	HPMUserContext m_UserContext;
	HPMString m_selectedTemplate;
	HPMString m_selectedProjectName;

	CHansoftCloneProject_ClientPlugin(const void *_pClientData)
	{
		m_pSession = nullptr;

		try
		{
			m_pSession = HPMSdkSession::SessionOpen(hpm_str(""), 0, hpm_str(""), hpm_str(""), hpm_str(""), this, NULL, true, EHPMSdkDebugMode_Off, _pClientData, 0, hpm_str(""), HPMSystemString(), NULL);
			m_IntegrationIdentifier = hpm_str("com.hansoft.cloneproject.clientplugin");
			m_UserContext = (void *)23796;
		}
		catch (HPMSdkException &_Exception)
		{
			HPMString ErrorStr = _Exception.GetAsString();

			wstringstream Stream;
			Stream << hpm_str("Error initializing Clone Project Client Plugin:\r\n\r\n");
			Stream << ErrorStr;
			MessageBox(NULL, Stream.str().c_str(), hpm_str("Clone Project Client Plugin Error"), MB_OK|MB_ICONERROR);
		}
		catch (HPMSdkCppException &_Exception)
		{
			wstringstream Stream; 
			Stream << hpm_str("Error initializing Clone Project Client Plugin:\r\n\r\n");
			Stream << _Exception.what();
			MessageBox(NULL, Stream.str().c_str(), hpm_str("Clone Project Client Plugin Error"), MB_OK|MB_ICONERROR);
		}
	}

	~CHansoftCloneProject_ClientPlugin()
	{ 
		m_pDynamicHelper.reset();
		if (m_pSession)
		{
			HPMSdkSession::SessionDestroy(m_pSession);
			m_pSession = nullptr;
		}
	}

	virtual void On_ProcessError(EHPMError _Error)
	{
	}

	virtual void On_Callback(const HPMChangeCallbackData_ClientSyncDone &_Data)
	{
		m_CustomDialogSpec = 
			hpm_str("	DialogName \"Create project from template\"\r\n")
			hpm_str("	Item Tab\r\n")
			hpm_str("	{\r\n")
			hpm_str("		Identifier \"com.hansoft.cloneprojectclientplugin.createprojectdialog\"\r\n")
			hpm_str("		Name \"Info\"\r\n")
			hpm_str("		InfoText \"Custom dynamic SDK dialog\"\r\n")	
			hpm_str("		LayoutStyle \"VerticalList\"\r\n")
			hpm_str("		Item FormLayout\r\n")
			hpm_str("		{\r\n")
			hpm_str("			Identifier \"Form\"\r\n")
			hpm_str("			Item Edit\r\n")
			hpm_str("			{\r\n")
			hpm_str("				Identifier \"ProjectName\"\r\n")
			hpm_str("				Name \"Project name:\"\r\n")
			hpm_str("				DefaultValue \"\"\r\n")
			hpm_str("				Password 0\r\n")
			hpm_str("			}\r\n")
			hpm_str("		}\r\n")
			hpm_str("		Item FormLayout\r\n")
			hpm_str("		{\r\n")
			hpm_str("			Identifier \"Form2\"\r\n")
			hpm_str("		Item StaticText\r\n")
			hpm_str("		{\r\n")
			hpm_str("			Name \"Information\"\r\n")
			hpm_str("			Identifier \"EpicInfo\"\r\n")
			hpm_str("			DefaultValue \"A new Hansoft project with the given name will be created using the selected project as a template for tasks, reports, view presets, and workflows and pipelines.\"\r\n")
			hpm_str("		}\r\n")
			hpm_str("		}\r\n")
			hpm_str("	}\r\n");

		try
		{
			m_pDynamicHelper = shared_ptr<CDynamicHelper>(new CDynamicHelper());
			m_pDynamicHelper->m_RightClickSubscription = m_pSession->GlobalRegisterForRightClickNotifications(NULL);
			m_pDynamicHelper->m_DynamicUpdateSubscription = m_pSession->GlobalRegisterForDynamicCustomSettingsNotifications(hpm_str("com.hansoft.cloneprojectclientplugin."), m_UserContext);
		}
		catch (const HPMSdk::HPMSdkException &_Exception)
		{
			if (_Exception.GetError() == EHPMError_ConnectionLost)
				return;
		}
		catch (const HPMSdk::HPMSdkCppException &)
		{
		}
	}

	bool ProjectApplicable(HPMProjectProperties projectProps)
	{
		return (!projectProps.m_bArchivedStatus && projectProps.m_Name.compare(0, 11, hpm_str("Template - "))==0);
	}

	virtual void On_Callback(const HPMChangeCallbackData_RightClickDisplayTaskMenu &_Data)
	{
		try
		{
			// Only someone with Administrator rights is allowed to create new projects
			HPMUniqueID loggedInUser = m_pSession->ResourceGetLoggedIn();
			HPMResourceProperties resourceProps = m_pSession->ResourceGetProperties(loggedInUser);
			if (resourceProps.m_Flags & EHPMResourceFlag_AdminAccess)
			{
				m_pSession->GlobalAddRightClickMenuItem
					(
					_Data.m_RightClickContext
					, hpm_str(""), m_IntegrationIdentifier + hpm_str(".taskmenu.cloneprojectclientplugin.root")
					, m_pSession->LocalizationCreateUntranslatedStringFromString(hpm_str("Create project from template"))
					, NULL
					)
					;

				HPMProjectEnum projects = m_pSession->ProjectEnum();
				for (unsigned i=0; i<projects.m_Projects.size(); i+=1)
				{
					HPMProjectProperties properties = m_pSession->ProjectGetProperties(projects.m_Projects[i]);
					if (ProjectApplicable(properties))
						m_pSession->GlobalAddRightClickMenuItem
							(
							_Data.m_RightClickContext
							, m_IntegrationIdentifier + hpm_str(".taskmenu.cloneprojectclientplugin.root")
							, m_IntegrationIdentifier + hpm_str(".taskmenu.cloneprojectclientplugin." + properties.m_Name)
							, m_pSession->LocalizationCreateUntranslatedStringFromString(properties.m_Name)
							, NULL
							)
							;
				}
			}
		}
		catch (const HPMSdk::HPMSdkException &_Exception)
		{
			if (_Exception.GetError() == EHPMError_ConnectionLost)
				return;
		}
		catch (const HPMSdk::HPMSdkCppException &)
		{
		}
	}

	virtual void On_Callback(const HPMChangeCallbackData_ProjectResourceAdd &_Data)
	{
	}

	virtual void On_Callback(const HPMChangeCallbackData_RightClickMenuItemSelected &_Data)
	{
		if(_Data.m_UniqueName.compare(0, m_IntegrationIdentifier.size(), m_IntegrationIdentifier))
			return;
		try
		{
			m_selectedTemplate = hpm_str("");
			m_selectedProjectName = hpm_str("");
			HPMProjectEnum projects = m_pSession->ProjectEnum();
			for (unsigned i=0; i<projects.m_Projects.size(); i+=1)
			{
				HPMProjectProperties properties = m_pSession->ProjectGetProperties(projects.m_Projects[i]);
				if (_Data.m_UniqueName  == m_IntegrationIdentifier + hpm_str(".taskmenu.cloneprojectclientplugin.") + properties.m_Name)
					m_selectedTemplate = properties.m_Name;
			}

			HPMString InitialValues =
				hpm_str("com.hansoft.cloneprojectclientplugin.createprojectdialog\r\n")
				hpm_str("{\r\n")
				hpm_str("}");
			m_pSession->GlobalDisplayCustomSettingsDialog
				(
				HPMUniqueID()							// Can be set to specific project
				, HPMUniqueID()							// Can be set to specific resource
				, m_CustomDialogSpec
				, InitialValues
				)
				;
		}
		catch (const HPMSdk::HPMSdkException &_Exception)
		{
			if (_Exception.GetError() == EHPMError_ConnectionLost)
				return;
		}
		catch (const HPMSdk::HPMSdkCppException &)
		{
		}
	}

	virtual void On_Callback(const HPMChangeCallbackData_DynamicCustomSettingsValueChanged &_Data)
	{
		try
		{
			if (_Data.m_UserContext != m_UserContext)
				return;

			if (_Data.m_Path == hpm_str("com.hansoft.cloneprojectclientplugin.createprojectdialog/Form/ProjectName"))
			{
				m_selectedProjectName = _Data.m_Value;
			}
		}
		catch (const HPMSdk::HPMSdkException &_Exception)
		{
			if (_Exception.GetError() == EHPMError_ConnectionLost)
				return;
		}
		catch (const HPMSdk::HPMSdkCppException &)
		{
		}
	}

	virtual void On_Callback(const HPMChangeCallbackData_DynamicCustomSettingsNotification &_Data)
	{
		if (! (_Data.m_TabIdentifiers.size() == 1 && _Data.m_TabIdentifiers[0].compare(hpm_str("com.hansoft.cloneprojectclientplugin.createprojectdialog")) == 0))
			return;	


		if (_Data.m_Notification == EHPMDynamicCustomSettingsNotification_DialogEndedOk)
		{

			if ((m_selectedTemplate.compare(hpm_str("")) != 0) && (m_selectedProjectName.compare(hpm_str("")) != 0))
			{

				HPMProjectEnum projects = m_pSession->ProjectEnum();
				for (unsigned i=0; i<projects.m_Projects.size(); i+=1)
				{
					HPMProjectProperties properties = m_pSession->ProjectGetProperties(projects.m_Projects[i]);
					if (m_selectedTemplate == properties.m_Name)
					{
						sourceProjectID.m_ID = projects.m_Projects[i].m_ID;
						CloneProject(m_selectedProjectName);
					}
				}
			}
			else
			{
				MessageBox(NULL,  hpm_str("No project was created. You have to give a name for the project."), hpm_str("Hansoft Clone Project Plugin"), MB_OK|MB_ICONWARNING);
			}
		}
	}

	virtual void On_Callback(const HPMChangeCallbackData_ProjectCreate &_Data)
	{	
		if (_Data.m_TemporaryProjectID.m_ID == tempTargetProjectID.m_ID ||_Data.m_ProjectID.m_ID == tempTargetProjectID.m_ID)
		{
			HPMUniqueID sourceBacklogID = m_pSession->ProjectOpenBacklogProjectBlock(sourceProjectID);
			HPMUniqueID sourceQAProjectID = m_pSession->ProjectOpenQAProjectBlock(sourceProjectID);
			HPMProjectEnum projects = m_pSession->ProjectEnum();
			bool projectFound = false;
			wstringstream Stream; 
			Stream << hpm_str("targetProject ID : \r\n\r\n");
			Stream << _Data.m_ProjectID.m_ID;
			Stream << hpm_str("\r\n");

			for (unsigned i=0; i<projects.m_Projects.size(); i+=1) 
			{
				Stream << projects.m_Projects[i].m_ID;
				Stream << hpm_str(" ");

				if (projects.m_Projects[i].m_ID == _Data.m_ProjectID.m_ID)
				{
					projectFound = true;
					break;
				}
			}

			if (!projectFound) {
				MessageBox(NULL, hpm_str("It was not possible to fully create the project."), hpm_str("Failed to create project"), MB_OK|MB_ICONERROR);		
				return;
			}
			else
			{
				HPMProjectResourceProperties resourceProps;
				resourceProps.m_Flags = EHPMProjectResourceFlag_IsMainProjectManager;
				m_pSession->ProjectResourceAdd(_Data.m_ProjectID, m_pSession->ResourceGetLoggedIn(), resourceProps);
				MessageBox(NULL, hpm_str("The current user was added as main manager to the created project. It is reccommended that you log out and in again before continuing."), hpm_str("Project created"), MB_OK|MB_ICONINFORMATION);
				m_pSession->ProjectResourceSetProperties(_Data.m_ProjectID, m_pSession->ResourceGetLoggedIn(), resourceProps);
			}
		}
		tempTargetProjectID.m_ID = -1;

	}

	HPMUniqueID sourceProjectID;
	HPMUniqueID tempTargetProjectID;

	void CloneProject(HPMString newProjectName)
	{
        HPMProjectProperties templateProperties = m_pSession->ProjectGetProperties(sourceProjectID);
        templateProperties.m_Name = newProjectName;
		// TBD defuse this ugly hack when I understand how to this another way.
		templateProperties.m_SortName = m_selectedTemplate;
        tempTargetProjectID.m_ID = m_pSession->ProjectCreate(templateProperties).m_ID;
	}
};

CHansoftCloneProject_ClientPlugin *g_pClientPlugin;

extern "C" mod_export void DHPMSdkCallingConvention HPMClientSDKCreate(const void *_pClientData)
{
	g_pClientPlugin = new CHansoftCloneProject_ClientPlugin(_pClientData);
}

extern "C" mod_export void DHPMSdkCallingConvention HPMClientSDKDestroy()
{
	delete g_pClientPlugin;
}
