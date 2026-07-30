#include "Game/AttributeSetDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
void AttributeSetDefinition::LoadFromXmlElement( XmlElement const& element )
{
    XmlElement const* attributeElement = element.FirstChildElement( "Attribute" );
    GUARANTEE_OR_DIE( attributeElement, "AttributeSetDefinition::LoadFromXmlElement - attributeElement is not found" );

    while ( attributeElement )
    {
        std::string attributeElementName = attributeElement->Name();
        GUARANTEE_OR_DIE( attributeElementName == "Attribute", "Attribute is not Found" );

        std::string name = ParseXmlAttribute( *attributeElement, "name", "" );
        GUARANTEE_OR_DIE( !name.empty(), "AttributeSetDefinition::LoadFromXmlElement - name is not Found" );

        float value = ParseXmlAttribute( *attributeElement, "value", -1.f );
        GUARANTEE_OR_DIE( value >= 0.f, "AttributeSetDefinition::LoadFromXmlElement - value is not Found" );

        m_attributes[ name ] = value;

        attributeElement = attributeElement->NextSiblingElement( "Attribute" );
    }
}