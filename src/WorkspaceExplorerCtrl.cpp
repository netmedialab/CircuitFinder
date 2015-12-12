/////////////////////////////////////////////////////////////////////////////////
//
// Name        :WxWorkspaceExplorer.cpp
// Author      :MING
// Created     :2010-01-28 ���� 8:11:51
// Description :
// Copyright   :(C) 2009-1010 System Centroid Inc.
//
/////////////////////////////////////////////////////////////////////////////////

// Class automatically generated by Dev-C++ New Class wizard

#include <vector>
#include "WorkspaceExplorerCtrl.h" // class's header file
#include "CfDefines.h"
#include "ResourceUtil.h"
#include "LogMessageCtrl.h"
#include "FileUtil.h"
#include "systemimagelist.h"
#include "NewFileDialogCtrl.h"
#include "NewFileInitializer.h"

BEGIN_EVENT_TABLE(CfWorkspaceExplorerCtrl, wxTreeCtrl)
    EVT_TREE_ITEM_ACTIVATED(ID_WorkspaceExplorer, CfWorkspaceExplorerCtrl::OnItemActivated)
    EVT_TREE_ITEM_MENU(ID_WorkspaceExplorer, CfWorkspaceExplorerCtrl::OnItemMenu)
	EVT_TREE_KEY_DOWN(ID_WorkspaceExplorer, CfWorkspaceExplorerCtrl::OnPressKey)
	//EVT_TREE_BEGIN_LABEL_EDIT(ID_WorkspaceExplorer, CfWorkspaceExplorerCtrl::OnLabelEdit)
	//EVT_TREE_END_LABEL_EDIT(ID_WorkspaceExplorer, CfWorkspaceExplorerCtrl::OnEndLabelEdit)
	//EVT_TREE_SEL_CHANGED(ID_WorkspaceExplorer, CfWorkspaceExplorerCtrl::OnSelChanged)
	EVT_MENU(ID_WorkspaceExplorer_AddNewFile, CfWorkspaceExplorerCtrl::OnAddNewFile)
	EVT_MENU(ID_WorkspaceExplorer_AddExistingFile, CfWorkspaceExplorerCtrl::OnAddExistingFile)
	EVT_MENU(ID_WorkspaceExplorer_AddFolder, CfWorkspaceExplorerCtrl::OnAddFolder)
	EVT_MENU(ID_WorkspaceExplorer_AddClassifier, CfWorkspaceExplorerCtrl::OnAddClassifier)
	//EVT_MENU(ID_WorkspaceExplorer_BackupProject, CfWorkspaceExplorerCtrl::OnBackupProject)
	//EVT_MENU(ID_WorkspaceExplorer_BackupWorkspace, CfWorkspaceExplorerCtrl::OnBackupWorkspace)
	EVT_MENU(ID_WorkspaceExplorer_OpenFile, CfWorkspaceExplorerCtrl::OnOpenFile)
	EVT_MENU(ID_WorkspaceExplorer_RemoveItem, CfWorkspaceExplorerCtrl::OnRemoveItem)
	EVT_MENU(ID_WorkspaceExplorer_Rename, CfWorkspaceExplorerCtrl::OnRenameFolder)
	EVT_MENU(ID_WorkspaceExplorer_SetActiveProject, CfWorkspaceExplorerCtrl::OnSetActiveProject)
	EVT_MENU(ID_WorkspaceExplorer_CloseProject, CfWorkspaceExplorerCtrl::OnCloseProject)
	EVT_MENU(ID_WorkspaceExplorer_SaveProject, CfWorkspaceExplorerCtrl::OnSaveProject)
	//EVT_MENU(ID_WorkspaceExplorer_OpenProperty, CfWorkspaceExplorerCtrl::OnOpenProperty)
	//EVT_MENU(ID_WorkspaceExplorer_OpenWSProperty, CfWorkspaceExplorerCtrl::OnOpenWSProperty)
	EVT_MENU(ID_WorkspaceExplorer_RefreshWS, CfWorkspaceExplorerCtrl::OnRefreshWS)
	EVT_MENU(ID_WorkspaceExplorer_SortProject, CfWorkspaceExplorerCtrl::OnSortProject)
	EVT_MENU(ID_WorkspaceExplorer_NewProject, CfWorkspaceExplorerCtrl::OnNewProject)
	EVT_MENU(ID_WorkspaceExplorer_OpenProject, CfWorkspaceExplorerCtrl::OnOpenProject)
END_EVENT_TABLE()


// class constructor
CfWorkspaceExplorerCtrl::CfWorkspaceExplorerCtrl(wxWindow *parent)
:wxTreeCtrl(parent, ID_WorkspaceExplorer,wxPoint(0,0), wxSize(200,300),wxTR_DEFAULT_STYLE | wxNO_BORDER | wxTR_MULTIPLE | wxTR_EDIT_LABELS)
//,m_pParent(parent)
,CfNotificationSender(parent)
{
	SetImageList(CfIconList::Get()->GetIconList());
	
//	LoadProjectItems();
	
}

// class destructor
CfWorkspaceExplorerCtrl::~CfWorkspaceExplorerCtrl()
{
	// insert your code here
}

void CfWorkspaceExplorerCtrl::AddChildTreeNode(wxTreeItemId id, CProjectItemList *child_list)
{
	if(!child_list) return;
	CProjectItemList::iterator iter;
	wxString name;
	wxTreeItemId iid;

	for(iter=child_list->begin();iter!=child_list->end();iter++)
	{
		CfProjectItem *item = (*iter);
		iid = AppendItem(id, item->GetItemName(), GetTypeImageIndex(item),-1,(wxTreeItemData*)(item));
		if((item->Type == PIT_FILE) && !::wxFileExists(item->Path)) SetItemTextColour(iid,*wxRED);
		AddChildTreeNode(iid,(*iter)->pChildList);
	}

}

void CfWorkspaceExplorerCtrl::AddProjectNode(CfProject *project)
{
	//int n = m_ProjectRootNodeMap.size();

	if(project && !project->IsEmpty())
	{
		wxTreeItemId project_node = project->GetTreeItemId();

		if(!project_node.IsOk())
		{
			wxString proj_name = project->GetProjectName();
			wxASSERT(project->GetRootItem()->ProjectPtr);
			project_node = AppendItem(m_RootId, proj_name, PIT_RPOJECT,-1,project->GetRootItem());
			//wxArrayTreeItemIds items;

			AddChildTreeNode(project_node,project->GetRootItem()->pChildList);
			Expand(project_node);
			Expand(m_RootId);
			//m_ProjectRootNodeMap[project_node]=project;
			m_ProjectRootNodeMap.insert(std::map<wxTreeItemIdValue,CfProject*>::value_type(project_node.m_pItem,project));
		}

		if(m_BoldId.IsOk()) SetItemBold(m_BoldId,false);
		SetItemBold(project_node);		
		m_BoldId = project_node;
		project->SetWorkspaceExplorerItemId(project_node);
	}
	
	//std::map<wxTreeItemIdValue,CfProject*>::iterator iter;
	//n = m_ProjectRootNodeMap.size();
	//for (iter = m_ProjectRootNodeMap.begin(); iter!=m_ProjectRootNodeMap.end();iter++)
	//	wxMessageBox(*(*iter).second->GetProjectPath(), _("About XEMP"), wxOK, this); 
	//n = n;


}

wxTreeItemId CfWorkspaceExplorerCtrl::FindTreeItemByName(const wxTreeItemId &parent_id, const wxString& name)
{
	wxTreeItemIdValue cookie = (void*)&name;
	wxTreeItemId id = this->GetFirstChild(parent_id,cookie);
	while (id.IsOk())
	{
		if(this->GetItemText(id) == name) return id;
		id = this->GetNextChild(parent_id,cookie);
	}
	return NULL;
}

void CfWorkspaceExplorerCtrl::DeleteProjectNode(wxTreeItemId id)
{
	if(!id.IsOk())
		id = m_BoldId;

	if(id.IsOk())
	{
		CfProjectItem *item = (CfProjectItem *)GetItemData(id);
		if(item->Type != PIT_RPOJECT) return;

		CfProject *project = item->ProjectPtr;
		if(!project) return;
		
		m_pWorkspace->DeleteProject(project);  // delete project before delete node
		Delete(id);

		if(id == m_BoldId)
		{
			if(m_pWorkspace->GetProjectCount()>0)
			{
				wxTreeItemIdValue iv;
				m_BoldId = GetFirstChild(m_RootId,iv);
				SetItemBold(m_BoldId);
			}
		}
		SendInitEvent();
	}

}

int CfWorkspaceExplorerCtrl::GetTypeImageIndex(CfProjectItem *item)
{
	if(item->Type != PIT_FILE) return item->Type;
	return CfIconList::Get()->GetIconIndex(item->Type,item->Path);
//	wxString ext = item->GetItemName().AfterLast('.');
//	return CFileUtils::GetFileTypeIndex(ext) + 5; // 5 the offset of image index of FT_VERILOG

}
void CfWorkspaceExplorerCtrl::LoadWorkspace(CfWorkspace *pWorkspace)
{
	m_pWorkspace = pWorkspace;
	if(m_RootId.IsOk())	Delete(m_RootId);
	wxString ws_nm;
	ws_nm.Printf(_("Workspace '%s' (%d projects)"),m_pWorkspace->GetWorkspaceName().c_str(),m_pWorkspace->GetProjectCount());
	m_RootId = AddRoot(ws_nm,PIT_WORKSPACE,-1);
	for(size_t i=0;i<m_pWorkspace->GetProjectCount();i++)
	{
		AddProjectNode(m_pWorkspace->GetProject(i));
	}
	int index = m_pWorkspace->GetActiveProjectIndex();
	if( index != -1)
	{
		CfProject *proj = m_pWorkspace->GetProject(index);
		if(proj)
		{
			SetItemBold(m_BoldId,false);
			m_BoldId = proj->GetTreeItemId();
			SetItemBold(m_BoldId);
		}
	}
	SendInitEvent();

}
//
//
//void CfWorkspaceExplorerCtrl::OnLabelEdit(wxTreeEvent& event)
//{
//	CfProjectItem *item = (CfProjectItem *)GetItemData(event.GetItem());
//	if((!item) || ((item->Type != PIT_FOLDER) && (item->Type != PIT_CLASSIFIER) && (item->Type != OPEN_DIR)))
//		event.Veto();
//}
//
//void CfWorkspaceExplorerCtrl::OnEndLabelEdit(wxTreeEvent& event)
//{
//	if(event.IsEditCancelled()) return;
//
//	bool succuss = RenameFolder(event.GetItem(),event.GetLabel());
//	if(!succuss) 
//	{
//		event.Veto();
//		EndEditLabel(event.GetItem(),true);
//	}
//
//}
//
//void CfWorkspaceExplorerCtrl::OnContextMenu(wxContextMenuEvent& event)
//{
//    wxPoint pt = event.GetPosition();
//
//    wxLogDebug(wxT("OnContextMenu at screen coords (%i, %i)"), pt.x, pt.y);
//    
//	wxString open=wxT("test");
//	wxMenu menu(open);
//    menu.Append(ID_WorkspaceExplorer_OpenFile, open);
//    menu.AppendSeparator();
//
//    PopupMenu(&menu, pt);
//}
//
//
void CfWorkspaceExplorerCtrl::OnItemActivated(wxTreeEvent& event)
{
	wxArrayTreeItemIds selections;
	size_t n = GetSelections(selections);
	for(size_t i =0;i<n;i++)
	{
		wxTreeItemId itemId = selections[i];
		if(!itemId.IsOk()) continue;
		CfProjectItem *item = (CfProjectItem *)GetItemData(itemId);

		if(!item || (item->Type != PIT_FILE && item->Type != PIT_EXTERN_FILE))
		{
			if(IsExpanded(itemId)) Collapse(itemId);
			else Expand(itemId);
		}
		else
		{
			CfNotification *pNotification = new CfNotification(cfId_OpenFile);
			pNotification->SetString(item->Path);
			pNotification->SetData(item->ProjectPtr);
			AddNotification(pNotification);
		}
			//m_pFileNotebook->OpenFilePage(item->Path);

	}

}


void CfWorkspaceExplorerCtrl::OnPressKey(wxTreeEvent& event)
{
	if(event.GetKeyCode() == WXK_DELETE) OnRemoveItem(event);
}
void CfWorkspaceExplorerCtrl::OnItemMenu(wxTreeEvent& event)
{
    wxTreeItemId itemId = event.GetItem();
    CfProjectItem *item = itemId.IsOk() ? (CfProjectItem *)GetItemData(itemId): NULL;

    wxPoint clientpt = event.GetPoint();

	if(item) SelectItem(itemId);
    ShowMenu(item, clientpt);
    event.Skip();
}

void CfWorkspaceExplorerCtrl::OnAddExistingFile(wxCommandEvent& WXUNUSED(event))
{
	wxTreeItemId id;
	if(!m_bEvtFromTreeMenu) id = m_BoldId;
	else
	{
		wxArrayTreeItemIds selections;
		int n = GetSelections(selections);
		if(n>1) 
		{
			wxMessageBox(_("Please select one project!"), PROJECT_CAPTION, wxOK | wxICON_WARNING, this); 
			return;
		}


		id = selections[0];
	}

	if(id.IsOk())
	{
		CfProjectItem *item = (CfProjectItem *)GetItemData(id);
		CfProject *proj = item->ProjectPtr;
		wxString dir;
		if(proj)	dir = proj->GetDirectory();
		else		dir = item->GetDirectory();

		wxFileDialog dialog(this,
			_T("Add Existing File"),
			dir,
			_T(""),
			_T("All files (*.*)|*.*|C/C++ (*.c;*.cc;*.cpp;*.h)|*.c;*.cc;*.cpp;*.h|HDL (*.v;*.vhd;*.sv)|*.v;*.vhd;*.sv"),
			wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);

		//dialog.SetFilterIndex(1);

		if (dialog.ShowModal() == wxID_OK)
		{
			wxArrayString paths;
			dialog.GetPaths(paths);
			for(size_t i=0;i<paths.Count();i++)
			{
				AddProjectItem(id,paths[i],PIT_FILE);
				wxLogMessage(wxT("[File] Add file ") + paths[i]);
			}
		}
	}

}

void CfWorkspaceExplorerCtrl::OnAddNewFile(wxCommandEvent& WXUNUSED(event))
{
	wxTreeItemId id;
	if(!m_bEvtFromTreeMenu) id = m_BoldId;
	else
	{
		wxArrayTreeItemIds selections;
		int n = GetSelections(selections);
		if(n>1) 
		{
			wxMessageBox(_("Please select one project!"), PROJECT_CAPTION, wxOK | wxICON_WARNING, this); 
			return;
		}

		id = selections[0];
	}
	if(id.IsOk())
	{
		CfProjectItem *item = (CfProjectItem *)GetItemData(id);
		CfProject *proj = item->ProjectPtr;
		wxString dir;
		if(proj)	dir = proj->GetDirectory();
		else		dir = item->GetDirectory();

		//wxFileDialog dialog(this,
		//	_T("Create new file"),
		//	dir,
		//	_T(""),
		//	_T("All files (*.*)|*.*"),
		//	wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

		//dialog.SetFilterIndex(1);
		CfNewFileDialogCtrl dialog(this);
		if (dialog.ShowModal() == wxID_OK)
		{
			wxString path = dialog.GetFilePath();
			CfProjectItem *item = AddProjectItem(id,path,PIT_FILE);
			wxTextFile file(path);
			if(file.Create())
			{
				wxLogMessage(wxT("[File] Add file ") + path);
				CfNewFileInitializer nfi(path);
				nfi.InitNewFile();

				//Send notification to open file
				CfNotification *pNotification = new CfNotification(cfId_OpenFile);
				pNotification->SetString(path);
				pNotification->SetData(item->ProjectPtr);
				AddNotification(pNotification);
			}
			else
				wxLogMessage(_("Failed to create file '%s',%s"),path.c_str(),file.GetName());
		}
	}

}


void CfWorkspaceExplorerCtrl::OnAddFolder(wxCommandEvent& WXUNUSED(event))
{
	wxTreeItemId id;
	if(!m_bEvtFromTreeMenu) id = m_BoldId;
	else
	{
		wxArrayTreeItemIds selections;
		int n = GetSelections(selections);
		if(n>1) 
		{
			wxMessageBox(_("Please select one project!"), PROJECT_CAPTION, wxOK | wxICON_WARNING, this); 
			return;
		}

		id = selections[0];
	}
	if(id.IsOk())
	{
		wxTextEntryDialog dialog(this,
			_T("Please enter a folder name"),
			_T("New Folder"),
			_T("Folder Name"),
			wxOK | wxCANCEL);

		if (dialog.ShowModal() == wxID_OK)
		{
			AddProjectItem(id, dialog.GetValue(),PIT_FOLDER);
		}
	}

}


void CfWorkspaceExplorerCtrl::OnAddClassifier(wxCommandEvent& WXUNUSED(event))
{
	wxTreeItemId id;
	if(!m_bEvtFromTreeMenu) id = m_BoldId;
	else
	{
		wxArrayTreeItemIds selections;
		int n = GetSelections(selections);
		if(n>1) 
		{
			wxMessageBox(_("Please select one project!"), PROJECT_CAPTION, wxOK | wxICON_WARNING, this); 
			return;
		}

		id = selections[0];
	}
	if(id.IsOk())
	{
		wxTextEntryDialog dialog(this,
			//_T("This is a small sample\n")
			_T("Please enter a classifier name"),
			_T("New Classifier"),
			_T("Classifier Name"),
			wxOK | wxCANCEL);

		if (dialog.ShowModal() == wxID_OK)
		{
			AddProjectItem(id, dialog.GetValue(),PIT_CLASSIFIER);
			wxLogMessage(wxT("[File] Add Classifier ") + dialog.GetValue());
		}
	}

}

void CfWorkspaceExplorerCtrl::OnOpenFile(wxCommandEvent& WXUNUSED(event))
{
	wxArrayTreeItemIds selections;
	int n = GetSelections(selections);
	if(n==0) return;

	for(int i =0;i<n;i++)
	{
		wxTreeItemId id = selections[i];
		if(id.IsOk())
		{
			CfProjectItem *item = (CfProjectItem *)GetItemData(id);
//			wxLogMessage(wxT("[File] Open ") + GetItemText(id));
	//		m_pFileNotebook->OpenFilePage(item->Path);

			//Send notification to open file
			CfNotification *pNotification = new CfNotification(cfId_OpenFile);
			pNotification->SetString(item->Path);
			pNotification->SetData(item->ProjectPtr);
			AddNotification(pNotification);
		}
	}
}
void CfWorkspaceExplorerCtrl::OnRenameFolder(wxCommandEvent& WXUNUSED(event))
{
	wxString msg;
	wxArrayTreeItemIds selections;
	int n = GetSelections(selections);

	// if the node is root - workspace
	for(int i =0;i<n;i++)
	{
		if(m_RootId == selections[i]) return;
	}

	// if the node is project
	for(int i =0;i<n;i++)
	{
		CfProjectItem *item = (CfProjectItem *)GetItemData(selections[i]);
		if(!item) continue;
		if(item->Type == PIT_RPOJECT) return;
	}


	for(int i =0;i<n;i++)
	{
		wxTreeItemId id = selections[i];
		if(id.IsOk())
		{
			EditLabel(id);
		}
	}

}

void CfWorkspaceExplorerCtrl::OnRefreshWS(wxCommandEvent& event)
{	
	for(size_t i=0;i<m_pWorkspace->GetProjectCount();i++)
	{
		m_pWorkspace->GetProject(i)->UnsetTreeItemId();
	}

	CfNotification *pNotification = new CfNotification(cfId_RefreshWorkspace);
	AddNotification(pNotification);
}

void CfWorkspaceExplorerCtrl::OnRemoveItem(wxCommandEvent& WXUNUSED(event))
{
	wxString msg;
	wxArrayTreeItemIds selections;
	int n = GetSelections(selections);

	// if the node is root - workspace
	for(int i =0;i<n;i++)
	{
		if(m_RootId == selections[i]) return;
	}

	// if the node is project
	bool is_proj = false;
	for(int i =0;i<n;i++)
	{
		CfProjectItem *item = (CfProjectItem *)GetItemData(selections[i]);
		if(!item) continue;
		if(item->Type == PIT_RPOJECT) 
		{ 
			is_proj = true;
			msg << wxT("Close project '") << item->GetItemName() << wxT("'?");
			wxMessageDialog dlg(this,msg,PROJECT_CAPTION,wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION);
			int ok = dlg.ShowModal();
			if(ok != wxID_YES) continue;
			DeleteProjectNode(selections[i]); 
		}
		if(is_proj) return;
	}
	

	msg << wxT("Remove ") << n << wxT(" file(s) from project?");
	wxMessageDialog dlg(this,msg,PROJECT_CAPTION,wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION);
	int ok = dlg.ShowModal();
	if(ok != wxID_YES) return;

	for(int i =0;i<n;i++)
	{
		wxTreeItemId id = selections[i];
		if(id.IsOk())
		{
			RemoveItem(id);
		}
	}

}
void CfWorkspaceExplorerCtrl::OnNewProject(wxCommandEvent& event)
{
	wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_ID_NewProject);
	GetParent()->GetEventHandler()->ProcessEvent(evt);
//	m_pWorkspace->NewProjectDialog(m_pEnvironment);
}
void CfWorkspaceExplorerCtrl::OnOpenProject(wxCommandEvent& event)
{
//	PostAnEvent(GetParent(),EVT_UPDATE_STATUS_BAR,GetId(),this,0,wxT("Open Project..."));

	wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MENU_ID_OpenProject);
	GetParent()->GetEventHandler()->ProcessEvent(evt);
	//m_pWorkspace->OpenProjectDialog();
}

void CfWorkspaceExplorerCtrl::OnCloseProject(wxCommandEvent& event)
{
	//CfProject *project = NULL;


	//if(m_pFileNotebook->IfNeedSave())//check if there are files changed when close project, if there are, ask for save
	//	if(wxMessageBox(_("Save all the changes?"), PROJECT_CAPTION, wxICON_QUESTION | wxYES_NO, NULL) == wxYES)
	//		CWxFileNotebook::Get()->SaveAllFiles();


	wxArrayTreeItemIds selections;
	int n = GetSelections(selections);
	if(n==0) return;

	for(size_t i=0;i<n;i++)
	{
		wxTreeItemId id = selections[i];
		DeleteProjectNode(id);
	}
}
void CfWorkspaceExplorerCtrl::OnSaveProject(wxCommandEvent& event)
{

	wxArrayTreeItemIds selections;
	int n = GetSelections(selections);
	if(n==0) return;

	wxTreeItemId id = selections[0];
	if(id.IsOk())
	{
		CfProjectItem *item = (CfProjectItem *)GetItemData(id);
		CfProject *project = item->ProjectPtr;
		if(project)
			project->Dump();
	}

}
//void CfWorkspaceExplorerCtrl::OnOpenProperty(wxCommandEvent& event)
//{
//	wxTreeItemId id;
//	if(!m_bEvtFromTreeMenu) id = m_BoldId;
//	else
//	{
//		wxArrayTreeItemIds selections;
//		int n = GetSelections(selections);
//		if(n==0) return;
//
//		if(n>1) 
//		{
//			wxMessageBox(_("Please select one project!"), PROJECT_CAPTION, wxOK | wxICON_WARNING, this); 
//			return;
//		}
//
//		id = selections[0];
//	}
//
//	if(id.IsOk())
//	{
//		CfProjectItem *item = (CfProjectItem *)GetItemData(id);
//		//CfProject *project = item->ProjectPtr;
//		if(item)
//		{
//			CWxProjectPropertyDialog dlg(this,item);
//			dlg.ShowModal();
//			//wxLogMessage(project->GetProjectPath().c_str());
//			//wxCommandEvent evt(wxEVT_OPENPROJECTPROPERTY_EVENT, ID_Event_OpenProjectProperty);
//			//evt.SetClientData(project);
//			//GetEventHandler()->ProcessEvent(evt);
//		}
//	}
//
//}
//
//void CfWorkspaceExplorerCtrl::OnOpenWSProperty(wxCommandEvent& event)
//{
//	CfWorkspace *ws= CfWorkspace::Get();
//	if(ws)
//	{
//		CWxWorkspacePropertyDialog dlg(this,ws);
//		dlg.ShowModal();
//	}
//}
//
//void CfWorkspaceExplorerCtrl::OnRefreshWS(wxCommandEvent& event)
//{
//	wxCommandEvent evt(wxEVT_REFRESHWORKSPACE_EVENT, ID_Event_RefreshWorkspace);
//	GetEventHandler()->ProcessEvent(evt);
//
//}

void CfWorkspaceExplorerCtrl::OnSetActiveProject(wxCommandEvent& event)
{
	//CfProject *project = NULL;
	wxArrayTreeItemIds selections;
	int n = GetSelections(selections);
	if(n==0) return;
	if(n>1) 
	{
		wxMessageBox(_("Please select one project!"), PROJECT_CAPTION, wxOK | wxICON_WARNING, this); 
		return;
	}

	wxTreeItemId id = selections[0];
	SetActiveProject(id);

}

// Sort project by name
void CfWorkspaceExplorerCtrl::OnSortProject(wxCommandEvent& event)
{
	SortChildren(m_RootId);
	m_pWorkspace->SortProject();
}


CfProjectItem* CfWorkspaceExplorerCtrl::AddProjectItem(const wxTreeItemId& rood_id, wxString name,const CfItemType type, const wxString& parent_name)
{
	wxTreeItemId parent_id = rood_id;
	if(!parent_name.IsEmpty())
	{
		parent_id = FindTreeItemByName(parent_id,parent_name);
		if(!parent_id.IsOk()) parent_id = rood_id;
	}

	CfProjectItem *parent_item = (CfProjectItem *)GetItemData(parent_id);

	CfProject *project = parent_item->ProjectPtr;
	if(!project) return NULL;

	CfProjectItem* item = NULL;
	if(parent_item)
	{
		if(type == PIT_FOLDER)						//if folder, set absolute path
		{
			if(parent_item->Type == PIT_RPOJECT)
				name = parent_item->GetDirectory()+ _("\\") + name;
			else
				name = parent_item->Path+ _("\\") + name;
		}
		item = project->AddProjectItem(parent_item,name,type);
		if(item)
		{
			wxTreeItemId id = AppendItem(parent_id, item->GetItemName(), GetTypeImageIndex(item),-1,(wxTreeItemData*)item);
			UnselectAll();
			SelectItem(id);
			// MING_20110620
			if(type == PIT_FOLDER)
			{
				while(parent_item->Type== PIT_CLASSIFIER)
				{
					parent_item = parent_item->pParent;
				}
				if(wxMkDir(name) < 0)
					wxLogMessage(wxT("[File] Failed to create folder '%s'"),name.c_str());

			}
		}
		else
		{
			wxMessageDialog dlg(this,wxString::Format(wxT("Same item already exist! \n"+name.c_str())),PROJECT_CAPTION,wxOK | wxICON_WARNING);
			dlg.ShowModal();
		}
	}
	return item;	
}

//
//bool CfWorkspaceExplorerCtrl::RenameFolder(const wxTreeItemId id,const wxString new_name)
//{
//
//	CfProjectItem *item = (CfProjectItem *)GetItemData(id);
//	if(!item) return false;
//
//	CfProject *project = item->ProjectPtr;
//	if(!project) return false;
//
//	return project->RenameFolder(item,new_name);
//}
//

void CfWorkspaceExplorerCtrl::RemoveItem(const wxTreeItemId id)
{

	CfProjectItem *item = (CfProjectItem *)GetItemData(id);
	if(!item) return;
	if(item->Type == PIT_RPOJECT) { DeleteProjectNode(id); return;}

	CfProject *project = item->ProjectPtr;
	if(!project) return;

	//if(ok == wxID_YES)
	{
		if (!item) return;
		project->DelProjectItem(item);
		wxLogDebug(item->Path);
		Delete(id);
	}

}

void CfWorkspaceExplorerCtrl::ShowMenu(CfProjectItem *item, const wxPoint& pt)
{
    wxMenu menu;
	
	if(!item)   // root node
	{
		menu.Append(ID_WorkspaceExplorer_NewProject, wxT("&New Project..."));
		menu.Append(ID_WorkspaceExplorer_OpenProject, wxT("&Open Project..."));		
		menu.AppendSeparator();
		menu.Append(ID_WorkspaceExplorer_RefreshWS, _("&Refresh"));
		menu.Append(ID_WorkspaceExplorer_SortProject, _("&Sort"));
		//menu.AppendSeparator();
		//menu.Append(ID_WorkspaceExplorer_OpenWSProperty, _("&Property..."));
	}
	else
	{

		wxString text;
		int type;

		text << item->GetItemName();
		type = item->Type;

		if(type != PIT_FILE)
		{
			wxMenu* add_sub_menu = new wxMenu;
			add_sub_menu->Append(ID_WorkspaceExplorer_AddNewFile, wxT("Add New &File..."));
			add_sub_menu->Append(ID_WorkspaceExplorer_AddExistingFile, wxT("Add &Existing File..."));
			add_sub_menu->AppendSeparator();
			add_sub_menu->Append(ID_WorkspaceExplorer_AddClassifier, wxT("Add &Classifier..."));
#ifdef USE_FOLDER
			if(type != PIT_CLASSIFIER)
				add_sub_menu->Append(ID_WorkspaceExplorer_AddFolder, wxT("&Add New Folder..."));
#endif
			menu.AppendSubMenu(add_sub_menu,_("&Add"));

			//if((item->Type == PIT_FOLDER) || (item->Type == PIT_CLASSIFIER) || (item->Type == OPEN_DIR))
			//	menu.Append(ID_WorkspaceExplorer_Rename, _("&Rename"));
		}
		else
		{
			menu.Append(ID_WorkspaceExplorer_OpenFile, wxT("&Open '")+text+wxT("'"));
		}

		if(type != PIT_RPOJECT)
		{
			menu.Append(ID_WorkspaceExplorer_RemoveItem, wxT("&Remove\tDel"));
		}
		else
		{
			menu.AppendSeparator();
			wxString proj_name;
			CfProject *project = item->ProjectPtr;
			wxASSERT(project);
			if(project) proj_name = project->GetProjectName();
			wxString label;
			label.Printf(wxT("Activate &Project '%s'"),proj_name.c_str());
			menu.Append(ID_WorkspaceExplorer_SetActiveProject, label);

			menu.AppendSeparator();
			label.Printf(wxT("&Save Project '%s'"),proj_name.c_str());
			menu.Append(ID_WorkspaceExplorer_SaveProject, label);
			label.Printf(wxT("&Close Project '%s'"),proj_name.c_str());
			menu.Append(ID_WorkspaceExplorer_CloseProject, label);
		}
		menu.AppendSeparator();
		menu.Append(ID_WorkspaceExplorer_RefreshWS, _("&Refresh"));
		menu.Append(ID_WorkspaceExplorer_SortProject, _("&Sort"));
		//menu.AppendSeparator();
		//menu.Append(ID_WorkspaceExplorer_OpenProperty, _("&Property..."));
	}

	m_bEvtFromTreeMenu = true;    
	PopupMenu(&menu, pt);
	m_bEvtFromTreeMenu = false;
}

//
//CfProject* CfWorkspaceExplorerCtrl::GetProjectFromItem(const wxTreeItemId id)
//{
//    if (!id.IsOk()) return NULL;
//	std::map<wxTreeItemIdValue,CfProject*>::iterator iter = m_ProjectRootNodeMap.find(id.m_pItem);
//	if(iter!=m_ProjectRootNodeMap.end())
//		return m_ProjectRootNodeMap[id.m_pItem];
//	else
//		return GetProjectFromItem(GetItemParent(id));
//}
//
//
void CfWorkspaceExplorerCtrl::SendInitEvent()
{
	CfNotification *pNotification = new CfNotification(cfId_UpdateWinTitle);
	AddNotification(pNotification);
}



void CfWorkspaceExplorerCtrl::SetActiveProject(wxTreeItemId id)
{
	if(!id.IsOk()) return;
	if(id == m_BoldId) return;

	if(id.IsOk())
	{
		CfProjectItem *item = (CfProjectItem *)GetItemData(id);
		CfProject *project = item->ProjectPtr;
		if(!project) return;
		
		int index = m_pWorkspace->FindProject(project->GetProjectPath().c_str());

		m_pWorkspace->SetActiveProject(index);
		SetItemBold(m_BoldId,false);
		m_BoldId = id;
		SetItemBold(m_BoldId);

		//Update window title
		SendInitEvent();
	}
}
void CfWorkspaceExplorerCtrl::LoadProjectSuccessed()
{
//	CfProject* project = m_pWorkspace->GetActiveProject();
//	if(project) AddProjectNode(project);
}

void CfWorkspaceExplorerCtrl::NewFileDialog(const wxString& dir, CfProject* project)
{

}
//
//
//
//
