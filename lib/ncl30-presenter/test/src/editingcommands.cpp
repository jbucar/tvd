#include "../../src/impl/editingcommandevents.h"
#include <connector/handler/editingcommandhandler.h>
#include <gtest/gtest.h>
#include <string>

namespace ncl = ::br::pucrio::telemidia::ginga::ncl;

class EditingCommandsTest: public testing::Test
{
public:
	EditingCommandsTest() :
			ecd( NULL ) {
	}

	virtual ~EditingCommandsTest() {
	}

	virtual void SetUp() {
	}

	virtual void TearDown() {
		if (ecd != NULL
		)
			delete ecd;
	}

	void buildECD( unsigned char ctag, std::string privateDataPayload ) {
		if (ecd != NULL
		)
			delete ecd;
		ecd = new connector::EditingCommandData( ctag );
		ecd->addPayload( (const unsigned char*) privateDataPayload.c_str(), (int) privateDataPayload.size() );
	}

	connector::EditingCommandData *ecd;
};

TEST_F(EditingCommandsTest, AddDocumentECMinimal) {
	buildECD( ncl::EC_ADD_DOCUMENT, "BASE_ID" );

	ncl::AddDocumentEC* adec = new ncl::AddDocumentEC();
	ASSERT_FALSE( adec->parsePayload(ecd) );

	delete adec;
}

TEST_F(EditingCommandsTest, AddDocumentECBasic) {
	buildECD( ncl::EC_ADD_DOCUMENT, "BASE_ID,DOC_URI1,DOC_IOR1" );

	ncl::AddDocumentEC* adec = new ncl::AddDocumentEC();
	ASSERT_TRUE( adec->parsePayload(ecd) );
	EXPECT_EQ( adec->getBaseId(), "BASE_ID" );
	ASSERT_EQ( adec->getDocumentCount(), 1 );
	EXPECT_EQ( adec->getDocumentUri(0), "DOC_URI1" );
	EXPECT_EQ( adec->getDocumentIor(0), "DOC_IOR1" );

	delete adec;
}

TEST_F(EditingCommandsTest, AddDocumentECManyDocs) {
	buildECD( ncl::EC_ADD_DOCUMENT, "BASE_ID,DOC_URI1,DOC_IOR1,DOC_URI2,DOC_IOR2" );

	ncl::AddDocumentEC* adec = new ncl::AddDocumentEC();

	ASSERT_TRUE( adec->parsePayload(ecd) );
	EXPECT_EQ( adec->getBaseId(), "BASE_ID" );
	ASSERT_EQ( adec->getDocumentCount(), 2 );
	EXPECT_EQ( adec->getDocumentUri(0), "DOC_URI1" );
	EXPECT_EQ( adec->getDocumentIor(0), "DOC_IOR1" );
	EXPECT_EQ( adec->getDocumentUri(1), "DOC_URI2" );
	EXPECT_EQ( adec->getDocumentIor(1), "DOC_IOR2" );

	delete adec;
}

TEST_F(EditingCommandsTest, AddDocumentECManyManyDocs) {
	buildECD(
	        ncl::EC_ADD_DOCUMENT,
	        "BASE_ID,DOC_URI1,DOC_IOR1,DOC_URI1,DOC_IOR1,DOC_URI1,DOC_IOR1,DOC_URI1,DOC_IOR1,DOC_URI1,DOC_IOR1,DOC_URI1,DOC_IOR1,DOC_URI1,DOC_IOR1,DOC_URI1,DOC_IOR1,DOC_URI1,DOC_IOR1,DOC_URI1,DOC_IOR1" );

	ncl::AddDocumentEC* adec = new ncl::AddDocumentEC();

	ASSERT_TRUE( adec->parsePayload(ecd) );
	EXPECT_EQ( adec->getBaseId(), "BASE_ID" );
	ASSERT_EQ( adec->getDocumentCount(), 10 );
	for (int i = 0; i < 10; ++i) {
		EXPECT_EQ( adec->getDocumentUri(i), "DOC_URI1" );
		EXPECT_EQ( adec->getDocumentIor(i), "DOC_IOR1" );
	}

	delete adec;
}

TEST_F(EditingCommandsTest, AddDocumentECNoIOR) {
	buildECD( ncl::EC_ADD_DOCUMENT, "BASE_ID,DOC_URI1" );
	ncl::AddDocumentEC* adec = new ncl::AddDocumentEC();

	ASSERT_TRUE( adec->parsePayload(ecd) );
	EXPECT_EQ( adec->getBaseId(), "BASE_ID" );
	ASSERT_EQ( adec->getDocumentCount(), 1 );
	EXPECT_EQ( adec->getDocumentUri(0), "DOC_URI1" );
	EXPECT_EQ( adec->getDocumentIor(0), "" );

	delete adec;

	buildECD( ncl::EC_ADD_DOCUMENT, "BASE_ID,DOC_URI1,DOC_IOR1,DOC_URI2" );
	adec = new ncl::AddDocumentEC();

	ASSERT_TRUE( adec->parsePayload(ecd) );
	EXPECT_EQ( adec->getBaseId(), "BASE_ID" );
	ASSERT_EQ( adec->getDocumentCount(), 2 );
	EXPECT_EQ( adec->getDocumentUri(0), "DOC_URI1" );
	EXPECT_EQ( adec->getDocumentIor(0), "DOC_IOR1" );
	EXPECT_EQ( adec->getDocumentUri(1), "DOC_URI2" );
	EXPECT_EQ( adec->getDocumentIor(1), "" );

	delete adec;
}

TEST_F(EditingCommandsTest, SetPropertyECBasic) {
	buildECD( ncl::EC_SET_PROPERTY_VALUE, "BASE_ID,DOC_URI1,MEDIA1,top,50%" );

	ncl::SetPropertyValueEC* spvec = new ncl::SetPropertyValueEC();
	ASSERT_TRUE( spvec->parsePayload(ecd) );

	EXPECT_EQ( spvec->getBaseId(), "BASE_ID" );
	EXPECT_EQ( spvec->getDocumentId(), "DOC_URI1" );
	EXPECT_EQ( spvec->getNodeId(), "MEDIA1" );
	EXPECT_EQ( spvec->getPropertyId(), "top" );
	EXPECT_EQ( spvec->getPropertyValue(), "50%" );
	delete spvec;
}

TEST_F(EditingCommandsTest, DISABLED_SetPropertyECNullBase) {
	const char payload[] = ",DOC_URI1,MEDIA1,top,50%";
	buildECD( ncl::EC_SET_PROPERTY_VALUE, payload );

	ncl::SetPropertyValueEC* spvec = new ncl::SetPropertyValueEC();
	ASSERT_FALSE( spvec->parsePayload(ecd) );

	delete spvec;
}

TEST_F(EditingCommandsTest, DISABLED_SetPropertyECNullDoc) {
	buildECD( ncl::EC_SET_PROPERTY_VALUE, "BASE_ID,,MEDIA1,property_name,property_value" );

	ncl::SetPropertyValueEC* spvec = new ncl::SetPropertyValueEC();
	ASSERT_FALSE( spvec->parsePayload(ecd) );

	delete spvec;
}

TEST_F(EditingCommandsTest, DISABLED_SetPropertyECNullNode) {
	buildECD( ncl::EC_SET_PROPERTY_VALUE, "BASE_ID,DOC_ID,,property_name,property_value" );

	ncl::SetPropertyValueEC* spvec = new ncl::SetPropertyValueEC();
	ASSERT_FALSE( spvec->parsePayload(ecd) );

	delete spvec;
}

TEST_F(EditingCommandsTest, DISABLED_SetPropertyECNullProperty) {
	buildECD( ncl::EC_SET_PROPERTY_VALUE, "BASE_ID,DOC_ID,NODE_ID,,property_value" );

	ncl::SetPropertyValueEC* spvec = new ncl::SetPropertyValueEC();
	ASSERT_FALSE( spvec->parsePayload(ecd) );

	delete spvec;
}

TEST_F(EditingCommandsTest, DISABLED_SetPropertyECNullValue) {
	buildECD( ncl::EC_SET_PROPERTY_VALUE, "BASE_ID,DOC_ID,NODE_ID,property_name," );

	ncl::SetPropertyValueEC* spvec = new ncl::SetPropertyValueEC();
	ASSERT_FALSE( spvec->parsePayload(ecd) );

	delete spvec;
}

TEST_F(EditingCommandsTest, SetPropertyECCommaValue) {
	buildECD( ncl::EC_SET_PROPERTY_VALUE, "BASE_ID,DOC_ID,NODE_ID,top,50%,dato1,dato2" );

	ncl::SetPropertyValueEC* spvec = new ncl::SetPropertyValueEC();
	ASSERT_TRUE( spvec->parsePayload(ecd) );
	EXPECT_EQ( spvec->getBaseId(), "BASE_ID" );
	EXPECT_EQ( spvec->getDocumentId(), "DOC_ID" );
	EXPECT_EQ( spvec->getNodeId(), "NODE_ID" );
	EXPECT_EQ( spvec->getPropertyId(), "top" );
	EXPECT_EQ( spvec->getPropertyValue(), "50%,dato1,dato2" );

	delete spvec;
}
/*      No se sabe si este test tiene que fallar o pasar, ya que los argumentos se separan por comas...
 TEST_F(EditingCommandsTest, SetPropertyECCommaValue2) {
 std::string propValue = ",,,,,,,,,";
 buildECD(ncl::EC_SET_PROPERTY_VALUE, "BASE_ID,DOC_ID,NODE_ID,property_name," + propValue);
 
 ncl::SetPropertyValueEC* spvec = new ncl::SetPropertyValueEC();
 ASSERT_TRUE (spvec->parsePayload(ecd));

 EXPECT_EQ(spvec->getBaseId()       , "BASE_ID");
 EXPECT_EQ(spvec->getDocumentId()   , "DOC_ID");
 EXPECT_EQ(spvec->getNodeId()       , "NODE_ID");
 EXPECT_EQ(spvec->getPropertyId()   , "property_name");
 EXPECT_EQ(spvec->getPropertyValue(), propValue);

 delete spvec;
 }
 */TEST_F(EditingCommandsTest, SetPropertyECCommaValue3) {
	std::string propValue = ",testValue";
	buildECD( ncl::EC_SET_PROPERTY_VALUE, "BASE_ID,DOC_ID,NODE_ID,property_name," + propValue );

	ncl::SetPropertyValueEC* spvec = new ncl::SetPropertyValueEC();
	ASSERT_TRUE( spvec->parsePayload(ecd) );
	EXPECT_EQ( spvec->getBaseId(), "BASE_ID" );
	EXPECT_EQ( spvec->getDocumentId(), "DOC_ID" );
	EXPECT_EQ( spvec->getNodeId(), "NODE_ID" );
	EXPECT_EQ( spvec->getPropertyId(), "property_name" );
	EXPECT_EQ( spvec->getPropertyValue(), propValue );

	delete spvec;
}

TEST_F(EditingCommandsTest, SetPropertyECCharset) {
	std::string payload = "BASE_ID,DOC_ID,NODE_ID,property_name,";
	std::string propValue = "1234567890qwertyuiopasdfghjklñzxcvbnmQWERTYUIOPASDFGHJKLÑZXCVBNMáéíóúÁÉÍÓÚ"
			"!\"·$%%&/()=?¿|@#~½¬'¡\t\nºª\\ .,;:";

	buildECD( ncl::EC_SET_PROPERTY_VALUE, payload + propValue );

	ncl::SetPropertyValueEC* spvec = new ncl::SetPropertyValueEC();
	ASSERT_TRUE( spvec->parsePayload(ecd) );
	EXPECT_EQ( spvec->getBaseId(), "BASE_ID" );
	EXPECT_EQ( spvec->getDocumentId(), "DOC_ID" );
	EXPECT_EQ( spvec->getNodeId(), "NODE_ID" );
	EXPECT_EQ( spvec->getPropertyId(), "property_name" );
	EXPECT_EQ( spvec->getPropertyValue(), propValue );

	delete spvec;
}

TEST_F(EditingCommandsTest, SetPropertyECMaxSizeValue) {
	std::string payload = "BASE_ID,DOC_ID,NODE_ID,property_name,";
	std::string propValue( 30811, 'a' );

	buildECD( ncl::EC_SET_PROPERTY_VALUE, payload + propValue );

	ncl::SetPropertyValueEC* spvec = new ncl::SetPropertyValueEC();
	ASSERT_TRUE( spvec->parsePayload(ecd) );
	EXPECT_EQ( spvec->getBaseId(), "BASE_ID" );
	EXPECT_EQ( spvec->getDocumentId(), "DOC_ID" );
	EXPECT_EQ( spvec->getNodeId(), "NODE_ID" );
	EXPECT_EQ( spvec->getPropertyId(), "property_name" );
	EXPECT_EQ( spvec->getPropertyValue(), propValue );

	delete spvec;
}

TEST_F(EditingCommandsTest, SetPropertyECOverflowValue) {

	std::string payload = "BASE_ID,DOC_ID,NODE_ID,property_name,";
	std::string propValue( 100810, 'a' );

	buildECD( ncl::EC_SET_PROPERTY_VALUE, payload + propValue );

	ncl::SetPropertyValueEC* spvec = new ncl::SetPropertyValueEC();
	ASSERT_TRUE( spvec->parsePayload(ecd) );

	EXPECT_EQ( spvec->getBaseId(), "BASE_ID" );
	EXPECT_EQ( spvec->getDocumentId(), "DOC_ID" );
	EXPECT_EQ( spvec->getNodeId(), "NODE_ID" );
	EXPECT_EQ( spvec->getPropertyId(), "property_name" );
	EXPECT_EQ( spvec->getPropertyValue(), propValue );

	delete spvec;
}

TEST_F(EditingCommandsTest, SetPropertyECLongArgs) {
	std::string baseId( 6169, 'b' );
	std::string docId( 6169, 'd' );
	std::string nodeId( 6169, 'n' );
	std::string propName( 6169, 'p' );
	std::string propValue( 6169, 'v' );

	buildECD( ncl::EC_SET_PROPERTY_VALUE, baseId + "," + docId + "," + nodeId + "," + propName + "," + propValue );

	ncl::SetPropertyValueEC* spvec = new ncl::SetPropertyValueEC();
	ASSERT_TRUE( spvec->parsePayload(ecd) );

	EXPECT_EQ( spvec->getBaseId(), baseId );
	EXPECT_EQ( spvec->getDocumentId(), docId );
	EXPECT_EQ( spvec->getNodeId(), nodeId );
	EXPECT_EQ( spvec->getPropertyId(), propName );
	EXPECT_EQ( spvec->getPropertyValue(), propValue );

	delete spvec;
}

TEST_F(EditingCommandsTest, SetPropertyWithSpaces) {

	std::string payload = "BASE_ID,DOC_ ID,NODE _ID,property_name,saraza";
	buildECD( ncl::EC_SET_PROPERTY_VALUE, payload );

	ncl::SetPropertyValueEC* spvec = new ncl::SetPropertyValueEC();
	ASSERT_TRUE( spvec->parsePayload(ecd) );
	EXPECT_EQ( spvec->getBaseId(), "BASE_ID" );
	EXPECT_EQ( spvec->getDocumentId(), "DOC_ ID" );
	EXPECT_EQ( spvec->getNodeId(), "NODE _ID" );
	EXPECT_EQ( spvec->getPropertyId(), "property_name" );
	EXPECT_EQ( spvec->getPropertyValue(), "saraza" );

	delete spvec;
}

TEST_F(EditingCommandsTest, AddLinkECMinimal) {
	buildECD( ncl::EC_ADD_LINK, "BASE_ID" );

	ncl::AddLinkEC* alec = new ncl::AddLinkEC();
	ASSERT_FALSE( alec->parsePayload(ecd) );

	delete alec;
}

TEST_F(EditingCommandsTest, AddLinkECBasic) {
	std::string xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
			"<link id=\"linkk\" xconnector=\"onBeginStart\">"
			"<bind component=\"image\" interface=\"medicalArea\" role=\"onBegin\"/>"
			"<bind component=\"image2\" role=\"start\"/>";
	buildECD( ncl::EC_ADD_LINK, "BASE_ID,DOC_ID,BODY," + xml );

	ncl::AddLinkEC* alec = new ncl::AddLinkEC();
	ASSERT_TRUE( alec->parsePayload(ecd) );
	EXPECT_EQ( alec->getBaseId(), "BASE_ID" );
	EXPECT_EQ( alec->getDocumentId(), "DOC_ID" );
	EXPECT_EQ( alec->getCompositeId(), "BODY" );
	EXPECT_EQ( alec->getXmlLink(), xml );

	delete alec;
}

