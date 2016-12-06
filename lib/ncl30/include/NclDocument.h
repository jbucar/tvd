/******************************************************************************
 Este arquivo eh parte da implementacao do ambiente declarativo do middleware
 Ginga (Ginga-NCL).

 Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

 Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
 os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
 Software Foundation.

 Este programa eh distribuido na expectativa de que seja util, porem, SEM
 NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
 ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
 GNU versao 2 para mais detalhes.

 Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
 com este programa; se nao, escreva para a Free Software Foundation, Inc., no
 endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

 Para maiores informacoes:
 ncl @ telemidia.puc-rio.br
 http://www.ncl.org.br
 http://www.ginga.org.br
 http://www.telemidia.puc-rio.br
 ******************************************************************************
 This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

 Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.
 Copyright 2010 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata


 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License version 2 as published by
 the Free Software Foundation.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
 details.

 You should have received a copy of the GNU General Public License version 2
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

 For further information contact:
 ncl @ telemidia.puc-rio.br
 http://www.ncl.org.br
 http://www.ginga.org.br
 http://www.telemidia.puc-rio.br
 *******************************************************************************/

#ifndef NCLDOCUMENT_H_
#define NCLDOCUMENT_H_

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace descriptor {
class DescriptorBase;
class GenericDescriptor;
}
namespace connectors {
class ConnectorBase;
class Connector;
}
namespace components {
class ContextNode;
class Node;
}
namespace switches {
class RuleBase;
class Rule;
}
namespace layout {
class LayoutRegion;
class RegionBase;
}
namespace transition {
class TransitionBase;
class Transition;
}
namespace metainformation {
class Meta;
class Metadata;
}
}
}
}
}
namespace bptnd = ::br::pucrio::telemidia::ncl::descriptor;
namespace bptncon = ::br::pucrio::telemidia::ncl::connectors;
namespace bptnc = ::br::pucrio::telemidia::ncl::components;
namespace bptns = ::br::pucrio::telemidia::ncl::switches;
namespace bptnl = ::br::pucrio::telemidia::ncl::layout;
namespace bptnt = ::br::pucrio::telemidia::ncl::transition;
namespace bptnm = ::br::pucrio::telemidia::ncl::metainformation;

#include <string>
#include <map>
#include <vector>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
  
/**
 * Esta es la clase principal del modelo NCL.
 * Representa al documento. Tiene atributos como el body, regionBase, descriptorBase, connectorBase, etc.
 */
class NclDocument
{
private:
	bptnc::ContextNode* body;
	bptncon::ConnectorBase* connectorBase;
	bptnd::DescriptorBase* descriptorBase;
	std::map<std::string, NclDocument*>* documentAliases;
	std::vector<NclDocument*>* documentBase;
	std::map<std::string, NclDocument*>* documentLocations;
	std::string id;
	std::map<int, bptnl::RegionBase*>* regionBases;
	bptns::RuleBase* ruleBase;
	bptnt::TransitionBase* transitionBase;
	std::vector<bptnm::Meta*>* metainformationList;
	std::vector<bptnm::Metadata*>* metadataList;

public:
	explicit NclDocument( const std::string & idParam );
	~NclDocument();
	bool addDocument( NclDocument* document, const std::string & alias, const std::string & location );
	void clear();
	bptncon::Connector* getConnector( const std::string & connectorId );
	bptncon::ConnectorBase* getConnectorBase();
	bptnt::Transition* getTransition( const std::string & transitionId );
	bptnt::TransitionBase* getTransitionBase();
	bptnd::GenericDescriptor* getDescriptor( const std::string & descriptorId );
	bptnd::DescriptorBase* getDescriptorBase();
	NclDocument* getDocument( const std::string & documentId );
	std::string getDocumentAlias( NclDocument* document );
	bptnc::ContextNode* getBody();
	std::string getDocumentLocation( NclDocument* document );
	std::vector<NclDocument*>* getDocuments();
	const std::string &getId();

private:
	bptnc::Node* getNodeLocally( const std::string & nodeId );

public:
	bptnc::Node* getNode( const std::string & nodeId );
	layout::LayoutRegion* getRegion( const std::string & regionId );

private:
	bptnl::LayoutRegion* getRegion( const std::string & regionId, bptnl::RegionBase* regionBase );

public:
	bptnl::RegionBase* getRegionBase( int devClass );
	bptnl::RegionBase* getRegionBase( const std::string & regionBaseId );
	std::map<int, bptnl::RegionBase*>* getRegionBases();
	bptns::Rule* getRule( const std::string & ruleId );
	bptns::RuleBase* getRuleBase();
	bool removeDocument( NclDocument* document );
	void setConnectorBase( bptncon::ConnectorBase* connectorBase );
	void setTransitionBase( bptnt::TransitionBase* transitionBase );
	void setDescriptorBase( bptnd::DescriptorBase* descriptorBase );
	void setDocumentAlias( NclDocument* document, const std::string & alias );
	void setBody( bptnc::ContextNode* node );
	void setDocumentLocation( NclDocument* document, const std::string & location );
	void setId( const std::string & id );
	void addRegionBase( bptnl::RegionBase* regionBase );
	void setRuleBase( bptns::RuleBase* ruleBase );
	void addMetainformation( bptnm::Meta* meta );
	void addMetadata( bptnm::Metadata* metadata );
	std::vector<bptnm::Meta*>* getMetainformation();
	std::vector<bptnm::Metadata*>* getMetadata();
	void removeRegionBase( const std::string & regionBaseId );
	void removeMetainformation( bptnm::Meta* meta );
	void removeMetadata( bptnm::Metadata* metadata );
	void removeAllMetainformation();
	void removeAllMetadata();
};
}
}
}
}

#endif /*NCLDOCUMENT_H_*/
