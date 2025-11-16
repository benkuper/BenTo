// Minimal browser-only OSC pack/unpack
// Supports: address + args [{ type, value }], no bundles
// Types: i (int32), f (float32), s (string), r (RGBA uint32),
//        T, F, N, I (no payload)

(function (global) {
  "use strict";

  var textEncoder = typeof TextEncoder !== "undefined" ? new TextEncoder() : null;
  var textDecoder = typeof TextDecoder !== "undefined" ? new TextDecoder() : null;

  function align4(n) {
    return (n + 3) & ~3;
  }

  function encodeString(str) {
    var bytes;
    if (textEncoder) {
      bytes = textEncoder.encode(str);
    } else {
      var arr = [];
      for (var i = 0; i < str.length; i++) {
        arr.push(str.charCodeAt(i) & 0xff);
      }
      bytes = new Uint8Array(arr);
    }

    var len = bytes.length + 1; // + null terminator
    var padded = align4(len);
    var out = new Uint8Array(padded);
    out.set(bytes, 0);
    // out[len-1] is already 0 by default
    return out;
  }

  function decodeString(view, offset) {
    var i = offset;
    var bytes = [];
    while (i < view.byteLength) {
      var b = view.getUint8(i);
      if (b === 0) break;
      bytes.push(b);
      i++;
    }

    var str;
    if (textDecoder) {
      str = textDecoder.decode(new Uint8Array(bytes));
    } else {
      var s = "";
      for (var j = 0; j < bytes.length; j++) {
        s += String.fromCharCode(bytes[j]);
      }
      str = s;
    }

    var nextOffset = align4(i + 1);
    return { value: str, nextOffset: nextOffset };
  }

  function packInt32(value) {
    var buf = new ArrayBuffer(4);
    var view = new DataView(buf);
    view.setInt32(0, value | 0, false); // big-endian
    return new Uint8Array(buf);
  }

  function packUint32(value) {
    var buf = new ArrayBuffer(4);
    var view = new DataView(buf);
    view.setUint32(0, value >>> 0, false); // big-endian
    return new Uint8Array(buf);
  }

  function packFloat32(value) {
    var buf = new ArrayBuffer(4);
    var view = new DataView(buf);
    view.setFloat32(0, value, false); // big-endian
    return new Uint8Array(buf);
  }

  function pack(msg) {
    if (!msg || typeof msg.address !== "string") {
      throw new Error("OscMini.pack: msg.address is required");
    }

    var args = msg.args || [];
    var typeTags = ",";
    for (var i = 0; i < args.length; i++) {
      typeTags += args[i].type;
    }

    var chunks = [];
    var totalLen = 0;

    // Address
    var addrBytes = encodeString(msg.address);
    chunks.push(addrBytes);
    totalLen += addrBytes.length;

    // Type tag string
    var tagBytes = encodeString(typeTags);
    chunks.push(tagBytes);
    totalLen += tagBytes.length;

    // Arguments
    for (var j = 0; j < args.length; j++) {
      var a = args[j];
      var t = a.type;
      var v = a.value;
      var data = null;

      switch (t) {
        case "i":
          data = packInt32(v || 0);
          break;
        case "f":
          data = packFloat32(v || 0.0);
          break;
        case "s":
          data = encodeString(v != null ? String(v) : "");
          break;
        case "r":
          // OSC 'r' = 32-bit RGBA color
          // Expect v as 0xRRGGBBAA uint32
          data = packUint32(v || 0);
          break;
        // T, F, N, I carry no bytes
        case "T":
        case "F":
        case "N":
        case "I":
          data = null;
          break;
        default:
          // Unknown type: skip
          data = null;
      }

      if (data) {
        chunks.push(data);
        totalLen += data.length;
      }
    }

    var out = new Uint8Array(totalLen);
    var offset = 0;
    for (var k = 0; k < chunks.length; k++) {
      out.set(chunks[k], offset);
      offset += chunks[k].length;
    }

    return out.buffer;
  }

  function unpack(buffer) {
    var view = new DataView(buffer);
    var offset = 0;

    // Address
    var addrParsed = decodeString(view, offset);
    var address = addrParsed.value;
    offset = addrParsed.nextOffset;

    // Type tags
    var tagsParsed = decodeString(view, offset);
    var typeTags = tagsParsed.value; // starts with ','
    offset = tagsParsed.nextOffset;

    if (!typeTags || typeTags[0] !== ",") {
      throw new Error("OscMini.unpack: invalid type tag string");
    }

    var args = [];
    for (var i = 1; i < typeTags.length; i++) {
      var t = typeTags[i];
      var v;

      switch (t) {
        case "i":
          v = view.getInt32(offset, false);
          offset += 4;
          args.push({ type: "i", value: v });
          break;

        case "f":
          v = view.getFloat32(offset, false);
          offset += 4;
          args.push({ type: "f", value: v });
          break;

        case "s":
          var strParsed = decodeString(view, offset);
          v = strParsed.value;
          offset = strParsed.nextOffset;
          args.push({ type: "s", value: v });
          break;

        case "r":
          // 32-bit RGBA color
          v = view.getUint32(offset, false);
          offset += 4;
          args.push({ type: "r", value: v });
          break;

        case "T":
          args.push({ type: "T", value: true });
          break;

        case "F":
          args.push({ type: "F", value: false });
          break;

        case "N":
          args.push({ type: "N", value: null });
          break;

        case "I":
          args.push({ type: "I", value: Infinity });
          break;

        default:
          // Unknown type: bail or skip
          // Here we just stop parsing further args
          i = typeTags.length;
          break;
      }
    }

    return { address: address, args: args };
  }

  var OscMini = {
    pack: pack,
    unpack: unpack
  };

  if (typeof module !== "undefined" && module.exports) {
    module.exports = OscMini;
  } else {
    global.OscMini = OscMini;
  }
})(this);
