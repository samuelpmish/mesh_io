#include "zstr.hpp"
#include "mesh/io.hpp"
#include "util.hpp"
#include "base64.hpp"
#include "node_ordering.hpp"

namespace io {

#if 1
std::vector<uint8_t> compress(const std::vector<uint8_t>& data) {

    // Create a zlib stream for compression
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    
    // Initialize the stream for compression
    if (deflateInit(&stream, Z_DEFAULT_COMPRESSION) != Z_OK) {
        // Handle initialization error
        // You can use stream.msg to get error details
        return std::vector<uint8_t>();
    }
    
    // Input data
    stream.next_in = const_cast<Bytef*>(data.data());
    stream.avail_in = data.size();
    
    // Create a buffer to store compressed data
    std::vector<uint8_t> compressedData(compressBound(data.size())); // You can adjust the initial size
    stream.next_out = compressedData.data();
    stream.avail_out = compressedData.size();
    
    // Compress the data
    int ret;
    ret = deflate(&stream, Z_FINISH);
    
    // Check for compression errors
    if (ret != Z_STREAM_END) {
        // Handle compression error
        // You can use stream.msg to get error details
        deflateEnd(&stream);
        return std::vector<uint8_t>();
    }
    
    // Clean up the compression stream
    deflateEnd(&stream);
    
    // Resize the compressed data to the actual size
    compressedData.resize(stream.total_out);
    
    return compressedData;
}
#else
std::vector<uint8_t> compress(std::vector<uint8_t> & original_data) {

  std::stringbuf buffer;
  zstr::ostream compressor{&buffer};

  compressor.write((char*)&original_data, original_data.size());
  compressor << std::flush;

  auto compstr = buffer.str();
  return std::vector<uint8_t>(compstr.begin(), compstr.end());

}
#endif

template < typename T >
void append_to_byte_array(uint8_t * & ptr, const T & data) {
  std::memcpy(ptr, &data, sizeof(T));
  ptr += sizeof(T);
}

template < typename T >
void append_to_byte_array(uint8_t * & ptr, const std::vector<T> & data) {
  std::memcpy(ptr, &data[0], sizeof(T) * data.size());
  ptr += sizeof(T) * data.size();
}

template < typename new_type, typename old_type, size_t n >
std::vector< std::array< new_type, n > > convert(const std::vector< std::array< old_type, n > > & old_data) {
  std::vector< std::array< new_type, n > > new_data(old_data.size());
  for (std::size_t i = 0; i < old_data.size(); i++) {
    for (std::size_t j = 0; j < n; j++) {
      new_data[i][j] = new_type(old_data[i][j]);
    }
  }
  return new_data;
}

template < typename new_type, typename old_type >
std::vector< new_type > convert(const std::vector< old_type > & old_data) {
  std::vector< new_type > new_data(old_data.size());
  for (std::size_t i = 0; i < old_data.size(); i++) {
    new_data[i] = new_type(old_data[i]);
  }
  return new_data;
}

std::string type_name(uint32_t) { return "UInt32"; }
std::string type_name(int32_t) { return "Int32"; }
std::string type_name(float) { return "Float32"; }
std::string type_name(double) { return "Float64"; }

template < typename float_t, typename int_t >
bool export_vtu_impl(const Mesh & mesh, std::string filename) {

  // binary data header is undocumented in VTK's official .vtu spec?
  // apparently for a single block of compressed data, these are the
  // appropriate values for the header:
  //
  // data = {...};
  // header[0] = 1;
  // header[1] = data.size();
  // header[2] = data.size();
  // header[3] = compress(data).size();
  //
  // output << Base64::encode(header) << Base64::encode(compress(data));
  std::vector<uint8_t> header_bytes(16);
  uint32_t * header = (uint32_t *)&header_bytes[0];

  int_t num_nodes = int_t(mesh.nodes.size());
  int_t num_elements = int_t(mesh.elements.size());

  std::ofstream outfile(filename, std::ios::binary | std::ios::trunc);

  outfile << "<?xml version=\"1.0\"?>\n";
  std::string byte_order = is_big_endian ? "BigEndian" : "LittleEndian";
  outfile << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"" << byte_order << "\" compressor=\"vtkZLibDataCompressor\">\n"; // TODO compressor
  outfile << "<UnstructuredGrid>\n";
  outfile << "<Piece NumberOfPoints=\"" << mesh.nodes.size() << "\" NumberOfCells=\"" << mesh.elements.size() << "\">\n";

  outfile << "<Points>\n";
  outfile << "<DataArray type=\"" << type_name(float_t{}) << "\" Name=\"Points\" NumberOfComponents=\"3\" format=\"binary\">\n";
  {
    uint32_t data_bytes = num_nodes * sizeof(float_t) * 3;
    std::vector<uint8_t> byte_vector(data_bytes);
    uint8_t * ptr = &byte_vector[0];
    append_to_byte_array(ptr, convert< float_t >(mesh.nodes));
    std::vector<uint8_t> compressed = compress(byte_vector);
    header[0] = 1;
    header[1] = byte_vector.size();
    header[2] = byte_vector.size();
    header[3] = compressed.size();
    outfile << Base64::Encode(header_bytes) << Base64::Encode(compressed) << '\n';
  }
  outfile << "</DataArray>\n";
  outfile << "</Points>\n";

  outfile << "<Cells>\n";
  outfile << "<DataArray type=\"" << type_name(int_t{}) << "\" Name=\"connectivity\" format=\"binary\">\n";
  {
    uint32_t data_bytes = 0;
    for (auto & elem : mesh.elements) {
      data_bytes += sizeof(int_t) * nodes_per_elem(elem.type);
    }
    std::vector<uint8_t> byte_vector(data_bytes);
    uint8_t * ptr = &byte_vector[0];
    for (auto & elem : mesh.elements) {
      for (int32_t i : vtk::permutation(elem.type)) {
        int_t id = elem.node_ids[i];
        append_to_byte_array(ptr, id);
      }
    }
    std::vector<uint8_t> compressed = compress(byte_vector);
    header[0] = 1;
    header[1] = byte_vector.size();
    header[2] = byte_vector.size();
    header[3] = compressed.size();
    outfile << Base64::Encode(header_bytes) << Base64::Encode(compressed) << '\n';
  }
  outfile << "</DataArray>\n";

  outfile << "<DataArray type=\"" << type_name(int_t{}) << "\" Name=\"offsets\" format=\"binary\">\n";
  {
    uint32_t data_bytes = num_elements * sizeof(int_t);
    std::vector<uint8_t> byte_vector(data_bytes);
    uint8_t * ptr = &byte_vector[0];
    int_t offset = 0;
    for (auto & elem : mesh.elements) {
      offset += nodes_per_elem(elem.type);
      append_to_byte_array(ptr, offset);
    }
    std::vector<uint8_t> compressed = compress(byte_vector);
    header[0] = 1;
    header[1] = byte_vector.size();
    header[2] = byte_vector.size();
    header[3] = compressed.size();
    outfile << Base64::Encode(header_bytes) << Base64::Encode(compressed) << '\n';
  }
  outfile << "</DataArray>\n";

  outfile << "<DataArray type=\"UInt8\" Name=\"types\" format=\"binary\">\n";
  {
    uint32_t data_bytes = num_elements;
    std::vector<uint8_t> byte_vector(data_bytes);
    uint8_t * ptr = &byte_vector[0];
    for (auto & elem : mesh.elements) {
      uint8_t vtk_id = vtk::element_type(elem.type);
      append_to_byte_array(ptr, vtk::element_type(elem.type));
    }
    std::vector<uint8_t> compressed = compress(byte_vector);
    header[0] = 1;
    header[1] = byte_vector.size();
    header[2] = byte_vector.size();
    header[3] = compressed.size();
    outfile << Base64::Encode(header_bytes) << Base64::Encode(compressed) << '\n';
  }
  outfile << "</DataArray>\n";
  outfile << "</Cells>\n";

  outfile << "</Piece>\n";
  outfile << "</UnstructuredGrid>\n";
  outfile << "</VTKFile>\n";

  outfile.close();

  return false;
}


bool export_vtu(const Mesh & mesh, std::string filename) {
  return export_vtu_impl<double, int32_t>(mesh, filename);
}

}