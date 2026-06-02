/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
    Original author: paulo <pmlopes@gmail.com>
*/

// You can use this small app to transform Collada mesh to Raydium TRI format.
// Materials are not exported (yet ?)

// Usage example: java Collada test.dae > test.tri
// (To compile, if you're not used to Java: javac Collada.java)

import java.io.File;
import java.io.IOException;
import java.util.*;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.xml.sax.SAXException;

public class Collada {

    private Document x;
    private List<Vertex> vertices;
    private List<Face> faces;
    private String name;
    private float scaleFactor;

    public Collada(Document xmlObj) {
        this.x = xmlObj;
        this.vertices = new ArrayList<Vertex>();
        this.faces = new ArrayList<Face>();
        this.name = "";
        this.scaleFactor = 15f;
    }

    public void getGeometry() {
        Node libGeom = XmlUtil.getChildByNodeName(x.getFirstChild(), "library_geometries");
        Node geom = libGeom.getChildNodes().item(1);
        this.name = geom.getAttributes().getNamedItem("name").getNodeValue();
        Node triangles = XmlUtil.getChildByNodeName(geom.getChildNodes().item(1), "triangles");
        Node vertSemantic = XmlUtil.getChildByNodeAndAttribute(triangles, "input", "semantic", "VERTEX");
        String vertexSource = vertSemantic.getAttributes().getNamedItem("source").getNodeValue().split("#")[1];
        String vertexOffset = vertSemantic.getAttributes().getNamedItem("offset").getNodeValue();
        Node vertices = XmlUtil.getChildByNodeName(geom.getChildNodes().item(1), "vertices");
        Node posSemantic = XmlUtil.getChildByNodeAndAttribute(vertices, "input", "semantic", "POSITION");
        String positions = posSemantic.getAttributes().getNamedItem("source").getNodeValue().split("#")[1];
        Node posSource = XmlUtil.getChildByNodeAndAttribute(geom.getChildNodes().item(1), "source", "id", positions);
        Node techCommon = XmlUtil.getChildByNodeName(posSource, "technique_common");
        Node accessor = XmlUtil.getChildByNodeName(techCommon, "accessor");
        String accSource = accessor.getAttributes().getNamedItem("source").getNodeValue().split("#")[1];
        Node posFloats = XmlUtil.getChildByNodeAndAttribute(posSource, "float_array", "id", accSource);
        String floats = "";
        int psLen = posFloats.getChildNodes().getLength();

        for (int i = 0; i < psLen; i++) {
            floats += posFloats.getChildNodes().item(i).getNodeValue();
        }
        String[] sposFloats = floats.split(" ");

        int len = sposFloats.length;

        // -- vertices

        for (int i = 0; i < len; i += 3) {
            this.vertices.add(new Vertex(
                    Float.parseFloat(sposFloats[i]) * this.scaleFactor * -1,
                    Float.parseFloat(sposFloats[i + 1]) * this.scaleFactor * -1,
                    Float.parseFloat(sposFloats[i + 2]) * this.scaleFactor));
        }

        // -- faces

        Node indices = XmlUtil.getChildByNodeName(triangles, "p");

        List<Integer> lindices = this.getIndicesByOffset(
                indices,
                Integer.parseInt(vertexOffset),
                this.getMaxOffset(triangles));

        len = lindices.size();

        for (int i = 0; i < len; i += 3) {
            this.faces.add(new Face(
                    this.vertices.get(lindices.get(i)),
                    this.vertices.get(lindices.get(i + 1)),
                    this.vertices.get(lindices.get(i + 2))));
        }
    }

    private List<Integer> getIndicesByOffset(Node indices, int offset, int maxOffset) {
        int len = indices.getChildNodes().getLength();
        List<Integer> out = new ArrayList<Integer>();
        String str = "";
        String[] sstr;

        for (int i = 0; i < len; i++) {
            str += indices.getChildNodes().item(i).getNodeValue();
        }

        sstr = str.split(" ");
        len = sstr.length;

        for (int i = offset; i < len; i += (maxOffset + 1)) {
            out.add(Integer.parseInt(sstr[i]));
        }

        return out;
    }

    private int getMaxOffset(Node triangles) {
        int len = triangles.getChildNodes().getLength();
        int maxOffset = 0;

        for (int i = 0; i < len; i++) {
            if (triangles.getChildNodes().item(i).getNodeName().equals("input")) {
                if (Integer.parseInt(triangles.getChildNodes().item(i).getAttributes().getNamedItem("offset").getNodeValue()) > maxOffset) {
                    maxOffset = Integer.parseInt(triangles.getChildNodes().item(i).getAttributes().getNamedItem("offset").getNodeValue());
                }
            }
        }

        return maxOffset;
    }

    public void exportTRI() {
        int len = faces.size();

	System.out.println("1");

        for (int i = 0; i < len; i++) {
            // -- flat shading
            Vertex normal = getFaceNormal(
                    faces.get(i).v0,
                    faces.get(i).v1,
                    faces.get(i).v2);
            
            System.out.printf("%f %f %f %f %f %f %f %f %s\n",
                    faces.get(i).v0.x / 3000f,
                    faces.get(i).v0.y / 3000f,
                    faces.get(i).v0.z / 3000f,
                    normal.x,
                    normal.y,
                    normal.z,
                    0f,
                    0f,
                    "rgb(0.5,0.5,0.5)"
                    );
            System.out.printf("%f %f %f %f %f %f %f %f %s\n",
                    faces.get(i).v1.x / 3000f,
                    faces.get(i).v1.y / 3000f,
                    faces.get(i).v1.z / 3000f,
                    normal.x,
                    normal.y,
                    normal.z,
                    0f,
                    0f,
                    "rgb(0.5,0.5,0.5)"
                    );
            System.out.printf("%f %f %f %f %f %f %f %f %s\n",
                    faces.get(i).v2.x / 3000f,
                    faces.get(i).v2.y / 3000f,
                    faces.get(i).v2.z / 3000f,
                    normal.x,
                    normal.y,
                    normal.z,
                    0f,
                    0f,
                    "rgb(0.5,0.5,0.5)"
                    );
        }
    }

    private Vertex getFaceNormal(Vertex vertex1, Vertex vertex2, Vertex vertex3) {

        Vertex u = new Vertex(
                vertex3.x - vertex2.x,
                vertex3.y - vertex2.y,
                vertex3.z - vertex2.z);
        Vertex v = new Vertex(
                vertex3.x - vertex1.x,
                vertex3.y - vertex1.y,
                vertex3.z - vertex1.z);
        Vertex n = new Vertex(
                (u.y * v.z - u.z * v.y),
                (u.z * v.x - u.x * v.z),
                (u.x * v.y - u.y * v.x));

        // -- normalizer
        float nLen = (float) Math.sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
        n.x = n.x / nLen;
        n.y = n.y / nLen;
        n.z = n.z / nLen;

        return n;
    }

    public static Document load(String filename, boolean validating) throws ParserConfigurationException, SAXException, IOException {
        // Create a builder factory
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        factory.setValidating(validating);

        // Create the builder and parse the file
        Document doc = factory.newDocumentBuilder().parse(new File(filename));
        return doc;
    }

    public static void main(String[] args) throws ParserConfigurationException, SAXException, IOException {
	if(args.length!=1)
	    {
	    System.out.println("You must provide a Collada mesh filename.");
	    System.exit(1);
	    }
	    
	Locale locale = Locale.getDefault();
	locale.setDefault(Locale.ENGLISH);
	
        Collada collada = new Collada(load(args[0], false));
        collada.getGeometry();
        collada.exportTRI();
    }
}

class Vertex {

    float x, y, z;

    public Vertex(float x, float y, float z) {
        this.x = x;
        this.y = y;
        this.z = z;
    }
}

class Face {

    Vertex v0, v1, v2;

    public Face(Vertex v0, Vertex v1, Vertex v2) {
        this.v0 = v0;
        this.v1 = v1;
        this.v2 = v2;
    }
}

class Point {

    float x, y;

    public Point(float x, float y) {
        this.x = x;
        this.y = y;
    }
}

class XmlUtil {

    public static Node getChildByNodeName(Node searchNode, String nodeName) {
        for (int i = 0; i < searchNode.getChildNodes().getLength(); i++) {
            if (searchNode.getChildNodes().item(i).getNodeName().equals(nodeName)) {
                return searchNode.getChildNodes().item(i);
            }
        }

        return null;
    }

    public static Node getChildByNodeAndAttribute(Node searchNode, String nodeName, String attrName, String attrValue) {
        for (int i = 0; i < searchNode.getChildNodes().getLength(); i++) {
            if (searchNode.getChildNodes().item(i).getNodeName().equals(nodeName) &&
                    searchNode.getChildNodes().item(i).getAttributes().getNamedItem(attrName).getNodeValue().equals(attrValue)) {
                return searchNode.getChildNodes().item(i);
            }
        }

        return null;
    }
}
